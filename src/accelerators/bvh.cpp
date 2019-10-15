#include <memory>
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
    auto rightAreaInv = (float*)buffer;
    auto totalAreaInv = 1 / node.bounds.surfaceArea();

    for (auto axis = 0; axis < 3; ++axis) {
      std::sort(start, end, [&, axis](auto a, auto b) {
        return primInfos[a].center[axis] < primInfos[b].center[axis];
      });
      Bounds3f bounds;
      for (std::uint32_t i = 1; i < nPrims; ++i) {
        bounds.extend(primInfos[*(end - i)].bounds);
        rightAreaInv[nPrims - i] = 1 / bounds.surfaceArea();
      }
      if (axis == 0)
        node.bounds = bounds.extend(primInfos[*start].bounds);
      bounds.reset();
      for (std::uint32_t i = 1; i < nPrims; ++i) {
        bounds.extend(primInfos[*(start + i - 1)].bounds);
        auto cost = 1 + totalAreaInv * (1 / bounds.surfaceArea() * i + rightAreaInv[i] * (nPrims - i));
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

  bool intersect(const Ray3f& ray) const override {
    auto mesh = meshes[0];
    for (std::uint32_t i = 0; i < mesh->getPrimitiveCount(); ++i) {
      if (mesh->intersect(i, ray)) return true;
    }
    return false;
  }

  bool intersect(const Ray3f& ray, Interaction& isect) const override {
    bool hit = false;
    auto mesh = meshes[0];
    std::uint32_t triIndex = 0;
    for (std::uint32_t i = 0; i < mesh->getPrimitiveCount(); ++i) {
      if (mesh->intersect(i, ray, isect)) {
        hit = true;
        triIndex = i;
      }
    }
    if (hit)
      mesh->computeIntersection(triIndex, isect);
    return hit;
  }

  std::string toString() const override {
    return "BVHAccel[]";
  }

private:
  std::vector<BVHNode> nodes;
};

MINPT_REGISTER_CLASS(BVHAccel, "bvh");

}
