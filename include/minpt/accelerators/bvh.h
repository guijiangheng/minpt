#pragma once

#include <minpt/core/accelerator.h>

namespace minpt {

class BVHAccel : public Accelerator {
public:
  void build() override
  { }

  Bounds3f getBoundingBox() const override {
    return Bounds3f();
  }

  bool intersect(const Ray3f& ray) const override;

  bool intersect(const Ray3f& ray, Interaction& isect) const override;

  std::string toString() const override {
    return "BVHAccel[]";
  }
};

}
