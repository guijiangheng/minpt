#include <memory>
#include <iostream>
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
    : bounds(bounds), splitAxis(splitAxis), rightChild(rightChild)
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
    exhaustBuild(primInfos.get(), 0u, indices.data(), indices.data() + nPrims, buffer.get());

    std::vector<BVHNode> packedNodes;
    auto cost = compactNodes(0, packedNodes);
    nodes = std::move(packedNodes);

    std::cout
      << "done (took " << timer.elapsedString() << " and "
      << memString(sizeof(BVHNode) * nodes.size() + sizeof(std::uint32_t) * indices.size())
      << ", SAH cost = " << cost << ")." << std::endl;
  }

  void exhaustBuild(
      PrimInfo* primInfos,
      std::uint32_t nodeIndex,
      std::uint32_t* start,
      std::uint32_t* end,
      std::uint32_t* buffer) {

    auto& node = nodes[nodeIndex];
    auto nPrims = (std::uint32_t)(end - start);

    if (nPrims == 1) {
      node = BVHNode(primInfos[*start].bounds, (std::uint32_t)(start - indices.data()), 1);
      return;
    }

    int splitIndex;
    int splitAxis = -1;
    float minCost = nPrims;
    auto rightAreas = (float*)buffer;
    auto totalAreaInv = 1 / node.bounds.surfaceArea();

    for (auto axis = 0; axis < 3; ++axis) {
      std::sort(start, end, [&, axis](auto a, auto b) {
        return primInfos[a].center[axis] < primInfos[b].center[axis];
      });
      Bounds3f bounds;
      for (std::uint32_t i = 1; i < nPrims; ++i) {
        bounds.extend(primInfos[*(end - i)].bounds);
        rightAreas[nPrims - i] = bounds.surfaceArea();
      }
      if (axis == 0)
        node.bounds = bounds.extend(primInfos[*start].bounds);
      bounds.reset();
      for (std::uint32_t i = 1; i < nPrims; ++i) {
        bounds.extend(primInfos[*(start + i - 1)].bounds);
        auto cost = INTERSECTION_COST + totalAreaInv * (bounds.surfaceArea() * i + rightAreas[i] * (nPrims - i));
        if (cost < minCost) {
          minCost = cost;
          splitIndex = i;
          splitAxis = axis;
        }
      }
    }

    if (splitAxis == -1) {
      node = BVHNode(node.bounds, (std::uint32_t)(start - indices.data()), (std::uint16_t)nPrims);
      return;
    }

    std::sort(start, end, [&, splitAxis](auto a, auto b) {
      return primInfos[a].center[splitAxis] < primInfos[b].center[splitAxis];
    });

    node.splitAxis = splitAxis;
    node.rightChild = nodeIndex + splitIndex * 2;
    exhaustBuild(primInfos, nodeIndex + 1, start, start + splitIndex, buffer);
    exhaustBuild(primInfos, node.rightChild, start + splitIndex, end, buffer + splitIndex);
  }

  float compactNodes(std::uint32_t nodeIndex, std::vector<BVHNode>& packedNodes) {
    auto& node = nodes[nodeIndex];
    packedNodes.push_back(node);

    if (node.nPrims)
      return 1.0f;

    auto rightChildIndex = node.rightChild;
    auto area = node.bounds.surfaceArea();
    auto leftArea = nodes[nodeIndex + 1].bounds.surfaceArea();
    auto rightArea = nodes[rightChildIndex].bounds.surfaceArea();
    auto leftCost = compactNodes(nodeIndex + 1, packedNodes);
    node.rightChild = (std::uint32_t)packedNodes.size();
    auto rightCost = compactNodes(rightChildIndex, packedNodes);
    return INTERSECTION_COST + (leftCost * leftArea + rightCost * rightArea) / area;
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
