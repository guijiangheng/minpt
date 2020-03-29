#pragma once

#include <minpt/core/accelerator.h>

namespace minpt {

struct KdTreeNode {
  union {
    float split;
    std::uint32_t onePrim;
    std::uint32_t primOffset;
  };

  union {
    std::uint32_t flags;
    std::uint32_t nPrims;
    std::uint32_t rightChild;
  };

  KdTreeNode() = default;

  // interior node
  KdTreeNode(std::uint32_t rightChild, int axis, float split) {
    this->split = split;
    flags = 3;
    this->rightChild = (rightChild << 2) | flags;
  }

  // leaf node
  KdTreeNode(std::uint32_t* prims, int nPrims, std::vector<std::uint32_t>& primIndices) {
    flags = 3;
    this->nPrims |= (nPrims << 2);
    if (nPrims == 0)
      onePrim = 0;
    else if (nPrims == 1)
      onePrim = prims[0];
    else {
      primOffset = primIndices.size();
      for (auto i = 0; i < nPrims; ++i)
        primIndices.push_back(prims[i]);
    }
  }
};

class KdTreeAccel : public Accelerator {
public:
  KdTreeAccel(const PropertyList& props);

  void build() override;

  std::string toString() const override {
    return tfm::format(
      "KdTreeAccel[\n"
      "  emptyBonus = %f"
      "]",
      emptyBonus
    );
  }

private:
  float emptyBonus;
};

}
