#pragma once

#include <minpt/core/sampler.h>

namespace minpt {

float fr(float cosThetaI, float eta);

struct BSDFQueryRecord {
  Vector3f wo;
  Vector3f wi;
  Vector3f p;
  Vector2f uv;
  float etaScale;
  Sampler* sampler;

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

  virtual Spectrum f(const BSDFQueryRecord& bRec) const = 0;

  virtual float pdf(const BSDFQueryRecord& bRec) const = 0;

  virtual Spectrum sample(BSDFQueryRecord& bRec, const Vector2f& u, float& pdf) const = 0;

  EClassType getClassType() const override {
    return EBSDF;
  }
};

}
