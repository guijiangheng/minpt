#pragma once

#include <minpt/core/accelerator.h>

namespace minpt {

struct BVHNode {
  Bounds3f bounds;
  std::uint16_t nPrims;
  std::uint16_t splitAxis;
  union {
    std::uint32_t primsOffset;
    std::uint32_t rightChild;
  };

  BVHNode() = default;

  // leaf node
  BVHNode(const Bounds3f& bounds, std::uint32_t primsOffset, std::uint16_t nPrims)
    : bounds(bounds), nPrims(nPrims), primsOffset(primsOffset)
  { }

  // interior node
  BVHNode(const Bounds3f& bounds, std::uint16_t splitAxis, std::uint32_t rightChild)
    : bounds(bounds), nPrims(0), splitAxis(splitAxis), rightChild(rightChild)
  { }
};

class BVHAccel : public Accelerator {
public:
  BVHAccel(const PropertyList& props)
  { }

  void build() override;

  std::pair<float, std::uint32_t> statistics(std::uint32_t nodeIndex) const;

  void compactNodes(std::uint32_t nodeIndex, std::vector<BVHNode>& packedNodes) const;

  bool intersect(const Ray& ray, Interaction& isect) const override;

  bool intersect(const Ray& ray) const override;

  std::string toString() const override {
    return "BVHAccel[]";
  }

private:
  std::vector<BVHNode> nodes;
};

}
