#include <iostream>
#include <memory>
#include <atomic>
#include <tbb/tbb.h>

#include <minpt/core/timer.h>
#include <minpt/accels/bvh.h>

namespace minpt {

struct PrimInfo {
  Bounds3f bounds;
  Vector3f center;

  PrimInfo() = default;

  PrimInfo(const Bounds3f& bounds) : bounds(bounds), center(bounds.centroid())
  { }
};

struct Bins {
  static constexpr auto BinCount = 16;
  std::uint32_t counts[BinCount];
  Bounds3f bounds[BinCount];

  Bins() {
    memset(counts, 0, sizeof(std::uint32_t) * BinCount);
  }
};

class BVHBuildTask : public tbb::task {
public:
  BVHBuildTask(
      std::vector<BVHNode>& nodes,
      const std::uint32_t* indices,
      const PrimInfo* primInfos,
      std::uint32_t nodeIndex,
      std::uint32_t* start, std::uint32_t* end, std::uint32_t* buffer)
    : nodes(nodes)
    , indices(indices)
    , primInfos(primInfos)
    , nodeIndex(nodeIndex)
    , start(start) , end(end) , buffer(buffer)
  { }

  tbb::task* execute() override {
    auto nPrims = (std::uint32_t)(end - start);

    if (nPrims < SerialThreshold) {
      seriallyBuild(nodeIndex, start, end, buffer);
      return nullptr;
    }

    auto& node = nodes[nodeIndex];
    auto axis = node.bounds.majorAxis();
    auto min = node.bounds.pMin[axis];
    auto max = node.bounds.pMax[axis];
    auto binSizeInv = Bins::BinCount / (max - min);

    auto bins = tbb::parallel_reduce(
      tbb::blocked_range<std::uint32_t>(0u, nPrims, GrainSize),
      Bins(),
      [=](const tbb::blocked_range<std::uint32_t>& range, Bins bins) -> Bins {
        for (auto i = range.begin(); i != range.end(); ++i) {
          auto f = start[i];
          auto centeroid = primInfos[f].center[axis];
          auto index = (int)((centeroid - min) * binSizeInv);
          if (index == Bins::BinCount) --index;
          ++bins.counts[index];
          bins.bounds[index].merge(primInfos[f].bounds);
        }
        return bins;
      },
      [](const Bins& a, const Bins& b) {
        Bins bins;
        for (auto i = 0; i < Bins::BinCount; ++i) {
          bins.counts[i] = a.counts[i] + b.counts[i];
          bins.bounds[i] = merge(a.bounds[i], b.bounds[i]);
        }
        return bins;
      }
    );

    std::uint8_t boundsBuffer[Bins::BinCount * sizeof(Bounds3f)];
    auto leftBounds = reinterpret_cast<Bounds3f*>(boundsBuffer);
    leftBounds[0] = bins.bounds[0];
    for (auto i = 1; i < Bins::BinCount - 1; ++i) {
      bins.counts[i] += bins.counts[i - 1];
      leftBounds[i] = merge(leftBounds[i - 1], bins.bounds[i]);
    }

    auto splitIndex = -1;
    auto minCost = (float)nPrims;
    auto totalAreaInv = 1 / node.bounds.area();
    Bounds3f bestRightBounds, rightBounds = bins.bounds[Bins::BinCount - 1];

    for (auto i = Bins::BinCount - 2; i >= 0; --i) {
      auto cost = TraversalCost + totalAreaInv * (
        bins.counts[i] * leftBounds[i].area() +
        (nPrims - bins.counts[i]) * rightBounds.area());
      if (cost < minCost) {
        minCost = cost;
        splitIndex = i;
        bestRightBounds = rightBounds;
      }
      rightBounds.merge(bins.bounds[i]);
    }

    if (splitIndex == -1) {
      seriallyBuild(nodeIndex, start, end, buffer);
      return nullptr;
    }

    auto leftCount = bins.counts[splitIndex];
    node.nPrims = 0;
    node.splitAxis = axis;
    node.rightChild = nodeIndex + 2 * leftCount;
    nodes[nodeIndex + 1].bounds = leftBounds[splitIndex];
    nodes[node.rightChild].bounds = bestRightBounds;

    std::atomic<std::uint32_t> offsetLeft(0);
    std::atomic<std::uint32_t> offsetRight(leftCount);

    tbb::parallel_for(
      tbb::blocked_range<std::uint32_t>(0u, nPrims, GrainSize),
      [=, &offsetLeft, &offsetRight](const tbb::blocked_range<std::uint32_t>& range) {
        std::uint32_t leftCount = 0;
        std::uint32_t rightCount = 0;
        for (auto i = range.begin(); i != range.end(); ++i) {
          auto f = start[i];
          auto centroid = primInfos[f].center[axis];
          auto index = (int)((centroid - min) * binSizeInv);
          if (index == Bins::BinCount) --index;
          ++(index <= splitIndex ? leftCount : rightCount);
        }
        auto left = offsetLeft.fetch_add(leftCount);
        auto right = offsetRight.fetch_add(rightCount);
        for (auto i = range.begin(); i != range.end(); ++i) {
          auto f = start[i];
          auto centroid = primInfos[f].center[axis];
          auto index = (int)((centroid - min) * binSizeInv);
          if (index == Bins::BinCount) --index;
          if (index <= splitIndex)
            buffer[left++] = f;
          else
            buffer[right++] = f;
        }
      }
    );

    memcpy(start, buffer, nPrims * sizeof(uint32_t));
    auto& c = *new(allocate_continuation()) tbb::empty_task();
    c.set_ref_count(2);
    auto& b = *new(c.allocate_child()) BVHBuildTask(
      nodes, indices, primInfos, node.rightChild, start + leftCount, end, buffer + leftCount);
    spawn(b);
    recycle_as_child_of(c);
    ++nodeIndex;
    end = start + leftCount;

    return this;
  }

  void seriallyBuild(std::uint32_t nodeIndex, std::uint32_t* start, std::uint32_t* end, std::uint32_t* buffer) {
    auto& node = nodes[nodeIndex];
    auto nPrims = (std::uint32_t)(end - start);

    if (nPrims == 1) {
      node = BVHNode(primInfos[*start].bounds, (std::uint32_t)(start - indices), 1);
      return;
    }

    int splitAxis = -1;
    std::uint32_t splitIndex = 0;
    float minCost = nPrims;
    float totalAreaInv;
    auto rightAreas = (float*)buffer;

    for (auto axis = 0; axis < 3; ++axis) {
      std::sort(start, end, [&, axis](auto a, auto b) {
        return primInfos[a].center[axis] < primInfos[b].center[axis];
      });
      Bounds3f bounds;
      for (std::uint32_t i = 1; i < nPrims; ++i) {
        bounds.merge(primInfos[*(end - i)].bounds);
        rightAreas[nPrims - i] = bounds.area();
      }
      if (axis == 0) {
        node.bounds = bounds.merge(primInfos[*start].bounds);
        totalAreaInv = 1 / node.bounds.area();
      }
      bounds.reset();
      for (std::uint32_t i = 1; i < nPrims; ++i) {
        bounds.merge(primInfos[*(start + i - 1)].bounds);
        auto cost = TraversalCost + (bounds.area() * i + rightAreas[i] * (nPrims - i)) * totalAreaInv;
        if (cost < minCost) {
          minCost = cost;
          splitIndex = i;
          splitAxis = axis;
        }
      }
    }

    if (splitAxis == -1) {
      node = BVHNode(node.bounds, (std::uint32_t)(start - indices), (std::uint16_t)nPrims);
      return;
    }

    std::sort(start, end, [&, splitAxis](auto a, auto b) {
      return primInfos[a].center[splitAxis] < primInfos[b].center[splitAxis];
    });

    node.splitAxis = splitAxis;
    node.rightChild = nodeIndex + splitIndex * 2;
    seriallyBuild(nodeIndex + 1, start, start + splitIndex, buffer);
    seriallyBuild(node.rightChild, start + splitIndex, end, buffer + splitIndex);
  }

public:
  static constexpr std::uint32_t SerialThreshold = 128;
  static constexpr std::uint32_t GrainSize = 1000;
  static constexpr float TraversalCost = 1.0f / 2;

private:
  std::vector<BVHNode>& nodes;
  const std::uint32_t* indices;
  const PrimInfo* primInfos;
  std::uint32_t nodeIndex;
  std::uint32_t* start;
  std::uint32_t* end;
  std::uint32_t* buffer;
};



void BVHAccel::build() {
  auto nPrims = getPrimitiveCount();
  if (!nPrims) return;

  std::cout
    << "Constructing a SAH BVH (" << meshes.size()
    << (meshes.size() == 1 ? " shape, " : " shapes, ")
    << nPrims << " primitives) .. ";

  Timer timer;

  auto primInfos = std::make_unique<PrimInfo[]>(nPrims);
  indices.resize(nPrims);
  auto primIndex = 0u;
  for (auto mesh : meshes)
    for (std::uint32_t i = 0, n = mesh->getPrimitiveCount(); i < n; ++i) {
      primInfos[primIndex] = PrimInfo(mesh->getBounds(i));
      indices[primIndex] = primIndex;
      ++primIndex;
    }

  auto buffer = std::make_unique<std::uint32_t[]>(nPrims);
  nodes.resize(nPrims * 2);
  nodes[0].bounds = bounds;
  auto& task = *new(tbb::task::allocate_root()) BVHBuildTask(
    nodes, indices.data(), primInfos.get(),
    0u, indices.data(), indices.data() + nPrims, buffer.get());
  tbb::task::spawn_root_and_wait(task);

  auto stats = statistics(0u);
  std::vector<BVHNode> packedNodes;
  packedNodes.reserve(stats.second);
  compactNodes(0u, packedNodes);
  nodes = std::move(packedNodes);

  std::cout
    << "done (took " << timer.elapsedString() << " and "
    << memString(sizeof(BVHNode) * nodes.size() + sizeof(std::uint32_t) * indices.size())
    << ", node count = " << stats.second
    << ", SAH cost = " << stats.first << ")." << std::endl;
}

std::pair<float, std::uint32_t> BVHAccel::statistics(std::uint32_t nodeIndex) const {
  auto& node = nodes[nodeIndex];
  if (node.nPrims)
    return std::make_pair((float)node.nPrims, 1u);
  auto left = statistics(nodeIndex + 1);
  auto right = statistics(node.rightChild);
  auto totalArea = node.bounds.area();
  auto leftArea = nodes[nodeIndex + 1].bounds.area();
  auto rightArea = nodes[node.rightChild].bounds.area();
  return std::make_pair(
    BVHBuildTask::TraversalCost + (left.first * leftArea + right.first * rightArea) / totalArea,
    left.second + right.second + 1
  );
}

void BVHAccel::compactNodes(std::uint32_t nodeIndex, std::vector<BVHNode>& packedNodes) const {
  packedNodes.push_back(nodes[nodeIndex]);
  auto& node = packedNodes.back();
  if (node.nPrims) return;
  compactNodes(nodeIndex + 1, packedNodes);
  auto rightChild = node.rightChild;
  node.rightChild = packedNodes.size();
  compactNodes(rightChild, packedNodes);
}

bool BVHAccel::intersect(const Ray& ray, Interaction& isect) const {
  if (nodes.empty()) return false;

  Vector3f invDir(1 / ray.d.x, 1 / ray.d.y, 1 / ray.d.z);
  const int dirIsNeg[3] = { invDir.x < 0, invDir.y < 0, invDir.z < 0 };

  auto hit = false;
  std::uint32_t index;
  std::uint32_t nodesToVisit[64];
  nodesToVisit[0] = 0u;
  std::uint32_t currentIndex;
  int toVisitOffset = 0;

  while (toVisitOffset != -1) {
    currentIndex = nodesToVisit[toVisitOffset--];
    auto& node = nodes[currentIndex];
    if (node.bounds.intersect(ray, invDir, dirIsNeg)) {
      if (node.nPrims) {
        for (std::uint32_t i = 0, n = (std::uint32_t)node.nPrims; i < n; ++i) {
          auto triIndex = indices[node.primsOffset + i];
          auto mesh = findMesh(triIndex);
          if (mesh->intersect(triIndex, ray, isect)) {
            index = triIndex;
            hit = true;
            isect.mesh = mesh;
          }
        }
      } else {
        if (dirIsNeg[node.splitAxis]) {
          nodesToVisit[++toVisitOffset] = currentIndex + 1;
          nodesToVisit[++toVisitOffset] = node.rightChild;
        } else {
          nodesToVisit[++toVisitOffset] = node.rightChild;
          nodesToVisit[++toVisitOffset] = currentIndex + 1;
        }
      }
    }
  }

  if (hit) {
    isect.wo = -ray.d;
    isect.mesh->computeIntersection(index, isect);
  }

  return hit;
}

bool BVHAccel::intersect(const Ray& ray) const {
  if (nodes.empty()) return false;

  Vector3f invDir(1 / ray.d.x, 1 / ray.d.y, 1 / ray.d.z);
  const int dirIsNeg[3] = { invDir.x < 0, invDir.y < 0, invDir.z < 0 };

  std::uint32_t nodesToVisit[64];
  nodesToVisit[0] = 0u;
  std::uint32_t currentIndex;
  int toVisitOffset = 0;

  while (toVisitOffset != -1) {
    currentIndex = nodesToVisit[toVisitOffset--];
    auto& node = nodes[currentIndex];
    if (node.bounds.intersect(ray, invDir, dirIsNeg)) {
      if (node.nPrims) {
        for (std::uint32_t i = 0, n = (std::uint32_t)node.nPrims; i < n; ++i) {
          auto triIndex = indices[node.primsOffset + i];
          auto mesh = findMesh(triIndex);
          if (mesh->intersect(triIndex, ray))
            return true;
        }
      } else {
        if (dirIsNeg[node.splitAxis]) {
          nodesToVisit[++toVisitOffset] = currentIndex + 1;
          nodesToVisit[++toVisitOffset] = node.rightChild;
        } else {
          nodesToVisit[++toVisitOffset] = node.rightChild;
          nodesToVisit[++toVisitOffset] = currentIndex + 1;
        }
      }
    }
  }

  return false;
}

}
