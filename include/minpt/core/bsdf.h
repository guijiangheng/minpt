#pragma once

#include <minpt/core/object.h>

namespace minpt {

class BSDF : public Object {
public:
  virtual bool isDelta() const {
    return false;
  }

  virtual Color3f f(const Vector3f& wo, const Vector3f& wi) const = 0;

  virtual float pdf(const Vector3f& wo, const Vector3f& wi) const = 0;

  virtual Color3f sample(
    const Vector2f& u,
    const Vector3f& wo,
    Vector3f& wi,
    float& pdf,
    float& etaScale) const = 0;

  EClassType getClassType() const override {
    return EBSDF;
  }
};

}
