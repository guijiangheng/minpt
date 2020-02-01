#pragma once

#include <minpt/core/bsdf.h>
#include <minpt/microfacets/trowbridge.h>

namespace minpt {

class RoughDielectric : public BSDF {
public:
  RoughDielectric(const PropertyList& props);

  Spectrum f(const BSDFQueryRecord& bRec) const override;

  float pdf(const BSDFQueryRecord& bRec) const override;

  Spectrum sample(BSDFQueryRecord& bRec, const Vector2f& u, float& pdf) const override;

  std::string toString() const override;

public:
  bool remapRoughness;
  Spectrum kr, kt;
  float intIOR;
  float extIOR;
  float eta;
  float uRoughness;
  float vRoughness;
  TrowbridgeReitzDistribution distrib;
};

}
