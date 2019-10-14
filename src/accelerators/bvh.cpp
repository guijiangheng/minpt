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
    auto primIndex = 0u;
    auto nPrims = getPrimitiveCount();
    auto primInfos = std::make_unique<PrimInfo[]>(nPrims);
    indices.resize(nPrims);
    for (auto mesh : meshes)
      for (std::uint32_t i = 0, n = mesh->getPrimitiveCount(); i < n; ++i) {
        primInfos[primIndex] = PrimInfo(mesh->getBoundingBox(i));
        indices[primIndex] = primIndex;
        ++primIndex;
      }
    nodes.resize(nPrims * 2);
    nodes[0].bounds = bounds;
    auto buffer = std::make_unique<std::uint32_t[]>(nPrims);
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
      node = BVHNode(primInfos[*start].bounds, *start, 1);
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
      for (std::uint32_t i = 0; i < nPrims - 1; ++i) {
        bounds.extend(primInfos[*(end - i)].bounds);
        rightAreaInv[i] = 1 / bounds.surfaceArea();
      }
      bounds.reset();
      for (std::uint32_t i = 0; i < nPrims - 1; ++i) {
        bounds.extend(primInfos[*(start + i)].bounds);
        auto cost = 1 + totalAreaInv * (1 / bounds.surfaceArea() * (i + 1) + rightAreaInv[i] * (nPrims - i - 1));
      }
    }


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
