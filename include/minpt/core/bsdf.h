#pragma once

#include <minpt/core/object.h>

namespace minpt {

struct BSDFQueryRecord {
  Vector3f wo;
  Vector3f wi;
  Vector3f p;
  Vector2f uv;
  float etaScale;

  BSDFQueryRecord(const Vector3f& wo) : wo(wo), etaScale(1.0f)
  { }

  BSDFQueryRecord(const Vector3f& wo, const Vector3f& wi) : wo(wo), wi(wi)
  { }
};

class BSDF : public Object {
public:
  virtual bool isDelta() const {
    return false;
  }

  virtual Color3f f(const BSDFQueryRecord& bRec) const = 0;

  virtual float pdf(const BSDFQueryRecord& bRec) const = 0;

  virtual Color3f sample(BSDFQueryRecord& bRec, const Vector2f& u, float& pdf) const = 0;

  EClassType getClassType() const override {
    return EBSDF;
  }
};

}
