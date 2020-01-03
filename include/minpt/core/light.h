#pragma once

#include <minpt/core/object.h>
#include <minpt/core/visibilitytester.h>

namespace minpt {

class Light : public Object {
public:
  virtual bool isDelta() const {
    return false;
  }

  virtual bool isInfinite() const {
    return false;
  }

  virtual Color3f sample(
    const Interaction& ref,
    const Vector2f& u,
    Vector3f& wi,
    float& pdf,
    VisibilityTester& tester) const = 0;

  EClassType getClassType() const override {
    return ELight;
  }
};

class InfiniteLight : public Light {
public:
  bool isInfinite() const override {
    return true;
  }

  virtual void preprocess(const Scene& scene)
  { }

  virtual float pdf(const Vector3f& w) const = 0;
};

}
