#include <iostream>
#include <memory>
#include <atomic>
#include <tbb/tbb.h>
#include <minpt/core/timer.h>
#include <minpt/core/accelerator.h>

namespace minpt {

struct PrimInfo {
  Bounds3f bounds;
  Vector3f center;

  PrimInfo() = default;

  PrimInfo(const Bounds3f& bounds) : bounds(bounds), center(bounds.center())
  { }
};

struct Bins {
  static constexpr auto BIN_COUNT = 16;
  std::uint32_t counts[BIN_COUNT];
  Bounds3f bounds[BIN_COUNT];

  Bins() {
    memset(counts, 0, sizeof(std::uint32_t) * BIN_COUNT);
  }
};

struct BVHNode {
  Bounds3f bounds;
  std::uint16_t nPrims;
  std::uint16_t splitAxis;
  union {
    std::uint32_t primsOffset;
    std::uint32_t rightChild;
  };

  BVHNode() = default;

  BVHNode(const Bounds3f& bounds, std::uint32_t primsOffset, std::uint16_t nPrims)
    : bounds(bounds), nPrims(nPrims), primsOffset(primsOffset)
  { }

  BVHNode(const Bounds3f& bounds, std::uint16_t splitAxis, std::uint32_t rightChild)
    : bounds(bounds), nPrims(0), splitAxis(splitAxis), rightChild(rightChild)
  { }
};

class BVHAccel : public Accelerator {
public:
  BVHAccel(const PropertyList& props)
  { }

  void build() override {
    auto nPrims = getPrimitiveCount();

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
        primInfos[primIndex] = PrimInfo(mesh->getBoundingBox(i));
        indices[primIndex] = primIndex;
        ++primIndex;
      }

    auto buffer = std::make_unique<std::uint32_t[]>(nPrims);
    nodes.resize(nPrims * 2);
    nodes[0].bounds = bounds;
    std::uint32_t totalNodes = 0;
    sahBuild(primInfos.get(), 0u, totalNodes, indices.data(), indices.data() + nPrims, buffer.get());

    std::vector<BVHNode> packedNodes;
    packedNodes.reserve(totalNodes);
    auto cost = compactNodes(0u, packedNodes);
    nodes = std::move(packedNodes);

    std::cout
      << "done (took " << timer.elapsedString() << " and "
      << memString(sizeof(BVHNode) * nodes.size() + sizeof(std::uint32_t) * indices.size())
      << ", SAH cost = " << cost << ")." << std::endl;
  }

  void sahBuild(
      PrimInfo* primInfos,
      std::uint32_t nodeIndex,
      std::uint32_t& totalNodes,
      std::uint32_t* start,
      std::uint32_t* end,
      std::uint32_t* buffer) {

    auto nPrims = (std::uint32_t)(end - start);

    if (nPrims < 64) {
      exhaustBuild(primInfos, nodeIndex, totalNodes, start, end, buffer);
      return;
    }

    auto& node = nodes[nodeIndex];
    auto axis = node.bounds.getMajorAxis();
    auto min = node.bounds.min()[axis];
    auto max = node.bounds.max()[axis];
    auto binSizeInv = Bins::BIN_COUNT / (max - min);

    auto bins = tbb::parallel_reduce(
      tbb::blocked_range<std::uint32_t>(0u, nPrims, 1000),
      Bins(),
      [=](const tbb::blocked_range<std::uint32_t>& range, Bins bins) -> Bins {
        for (auto i = range.begin(); i != range.end(); ++i) {
          auto f = start[i];
          auto centeroid = primInfos[f].center[axis];
          auto index = (int)((centeroid - min) * binSizeInv);
          if (index == Bins::BIN_COUNT) --index;
          ++bins.counts[index];
          bins.bounds[index].extend(primInfos[f].bounds);
        }
        return bins;
      },
      [](const Bins& a, const Bins& b) {
        Bins bins;
        for (auto i = 0; i < Bins::BIN_COUNT; ++i) {
          bins.counts[i] = a.counts[i] + b.counts[i];
          bins.bounds[i] = a.bounds[i].merged(b.bounds[i]);
        }
        return bins;
      }
    );

    Bounds3f leftBounds[Bins::BIN_COUNT];
    leftBounds[0] = bins.bounds[0];
    for (auto i = 1; i < Bins::BIN_COUNT - 1; ++i) {
      bins.counts[i] += bins.counts[i - 1];
      leftBounds[i] = bins.bounds[i].merged(leftBounds[i - 1]);
    }

    auto splitIndex = -1;
    auto minCost = (float)nPrims;
    auto totalAreaInv = 1 / node.bounds.surfaceArea();
    Bounds3f bestRightBounds, rightBounds = bins.bounds[Bins::BIN_COUNT - 1];

    for (auto i = Bins::BIN_COUNT - 2; i >= 0; --i) {
      auto cost = INTERSECTION_COST + totalAreaInv * (
        bins.counts[i] * leftBounds[i].surfaceArea() +
        (nPrims - bins.counts[i]) * rightBounds.surfaceArea());
      if (cost < minCost) {
        minCost = cost;
        splitIndex = i;
        bestRightBounds = rightBounds;
      }
      rightBounds.extend(bins.bounds[i]);
    }

    if (splitIndex == -1) {
      exhaustBuild(primInfos, nodeIndex, totalNodes, start, end, buffer);
      return;
    }

    ++totalNodes;
    auto leftCount = bins.counts[splitIndex];
    node.nPrims = 0;
    node.splitAxis = axis;
    node.rightChild = nodeIndex + 2 * leftCount;
    nodes[nodeIndex + 1].bounds = leftBounds[splitIndex];
    nodes[node.rightChild].bounds = bestRightBounds;

    std::atomic<std::uint32_t> offsetLeft(0);
    std::atomic<std::uint32_t> offsetRight(leftCount);

    tbb::parallel_for(
      tbb::blocked_range<std::uint32_t>(0u, nPrims, 1000),
      [=, &offsetLeft, &offsetRight](const tbb::blocked_range<std::uint32_t>& range) {
        std::uint32_t leftCount = 0;
        std::uint32_t rightCount = 0;
        for (auto i = range.begin(); i != range.end(); ++i) {
          auto f = start[i];
          auto centroid = primInfos[f].center[axis];
          auto index = (int)((centroid - min) * binSizeInv);
          if (index == Bins::BIN_COUNT) --index;
          ++(index <= splitIndex ? leftCount : rightCount);
        }
        auto left = offsetLeft.fetch_add(leftCount);
        auto right = offsetRight.fetch_add(rightCount);
        for (auto i = range.begin(); i != range.end(); ++i) {
          auto f = start[i];
          auto centroid = primInfos[f].center[axis];
          auto index = (int)((centroid - min) * binSizeInv);
          if (index == Bins::BIN_COUNT) --index;
          if (index <= splitIndex)
            buffer[left++] = f;
          else
            buffer[right++] = f;
        }
      }
    );

    ++totalNodes;
    memcpy(start, buffer, sizeof(std::uint32_t) * nPrims);
    sahBuild(primInfos, nodeIndex + 1, totalNodes, start, start + leftCount, buffer);
    sahBuild(primInfos, node.rightChild, totalNodes, start + leftCount, end, buffer);
  }

  void exhaustBuild(
      PrimInfo* primInfos,
      std::uint32_t nodeIndex,
      std::uint32_t& totalNodes,
      std::uint32_t* start,
      std::uint32_t* end,
      std::uint32_t* buffer) {

    auto& node = nodes[nodeIndex];
    auto nPrims = (std::uint32_t)(end - start);

    if (nPrims == 1) {
      ++totalNodes;
      node = BVHNode(primInfos[*start].bounds, (std::uint32_t)(start - indices.data()), 1);
      return;
    }

    int splitIndex;
    int splitAxis = -1;
    float minCost = nPrims;
    float totalAreaInv;
    auto rightAreas = (float*)buffer;

    for (auto axis = 0; axis < 3; ++axis) {
      std::sort(start, end, [&, axis](auto a, auto b) {
        return primInfos[a].center[axis] < primInfos[b].center[axis];
      });
      Bounds3f bounds;
      for (std::uint32_t i = 1; i < nPrims; ++i) {
        bounds.extend(primInfos[*(end - i)].bounds);
        rightAreas[nPrims - i] = bounds.surfaceArea();
      }
      if (axis == 0) {
        node.bounds = bounds.extend(primInfos[*start].bounds);
        totalAreaInv = 1 / node.bounds.surfaceArea();
      }
      bounds.reset();
      for (std::uint32_t i = 1; i < nPrims; ++i) {
        bounds.extend(primInfos[*(start + i - 1)].bounds);
        auto cost = INTERSECTION_COST + (bounds.surfaceArea() * i + rightAreas[i] * (nPrims - i)) * totalAreaInv;
        if (cost < minCost) {
          minCost = cost;
          splitIndex = i;
          splitAxis = axis;
        }
      }
    }

    if (splitAxis == -1) {
      ++totalNodes;
      node = BVHNode(node.bounds, (std::uint32_t)(start - indices.data()), (std::uint16_t)nPrims);
      return;
    }

    std::sort(start, end, [&, splitAxis](auto a, auto b) {
      return primInfos[a].center[splitAxis] < primInfos[b].center[splitAxis];
    });

    ++totalNodes;
    node.splitAxis = splitAxis;
    node.rightChild = nodeIndex + splitIndex * 2;
    exhaustBuild(primInfos, nodeIndex + 1, totalNodes, start, start + splitIndex, buffer);
    exhaustBuild(primInfos, node.rightChild, totalNodes, start + splitIndex, end, buffer + splitIndex);
  }

  float compactNodes(std::uint32_t nodeIndex, std::vector<BVHNode>& packedNodes) const {
    packedNodes.push_back(nodes[nodeIndex]);
    auto& node = packedNodes.back();
    if (node.nPrims) return 1.0f;
    auto rightChild = node.rightChild;
    auto totalArea = node.bounds.surfaceArea();
    auto leftArea = nodes[nodeIndex + 1].bounds.surfaceArea();
    auto rightArea = nodes[rightChild].bounds.surfaceArea();
    auto leftCost = compactNodes(nodeIndex + 1, packedNodes);
    node.rightChild = packedNodes.size();
    auto rightCost = compactNodes(rightChild, packedNodes);
    return INTERSECTION_COST + (leftCost * leftArea + rightCost * rightArea) / totalArea;
  }

  bool intersect(const Ray3f& ray, Interaction& isect) const override {
    Vector3f invDir(1 / ray.d.x(), 1 / ray.d.y(), 1 / ray.d.z());
    const int dirIsNeg[3] = { invDir.x() < 0, invDir.y() < 0, invDir.z() < 0 };

    int index;
    auto hit = false;
    int nodesToVisit[64];
    nodesToVisit[0] = 0;
    int currentIndex, toVisitOffset = 0;

    while (toVisitOffset != -1) {
      currentIndex = nodesToVisit[toVisitOffset--];
      auto& node = nodes[currentIndex];
      if (node.bounds.intersect(ray, invDir, dirIsNeg)) {
        if (node.nPrims) {
          for (auto i = 0; i < node.nPrims; ++i) {
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
      isect.mesh->computeIntersection(index, isect);
      isect.wo = -ray.d;
    }

    return hit;
  }

  bool intersect(const Ray3f& ray) const override {
    Vector3f invDir(1 / ray.d.x(), 1 / ray.d.y(), 1 / ray.d.z());
    const int dirIsNeg[3] = { invDir.x() < 0, invDir.y() < 0, invDir.z() < 0 };

    int nodesToVisit[64];
    nodesToVisit[0] = 0;
    int currentIndex, toVisitOffset = 0;

    while (toVisitOffset != -1) {
      currentIndex = nodesToVisit[toVisitOffset--];
      auto& node = nodes[currentIndex];
      if (node.bounds.intersect(ray, invDir, dirIsNeg)) {
        if (node.nPrims) {
          for (auto i = 0; i < node.nPrims; ++i) {
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

  std::string toString() const override {
    return "BVHAccel[]";
  }

private:
  std::vector<BVHNode> nodes;
  static constexpr auto INTERSECTION_COST = 1.0f;
};

MINPT_REGISTER_CLASS(BVHAccel, "bvh");

}
