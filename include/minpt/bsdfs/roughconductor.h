#pragma once


#include <minpt/core/bsdf.h>
#include <minpt/microfacets/trowbridge.h>

namespace minpt {

class RoughConductor : public BSDF {
public:
  RoughConductor(const PropertyList& props);

  Spectrum f(const BSDFQueryRecord& bRec) const override;

  float pdf(const BSDFQueryRecord& bRec) const override;

  Spectrum sample(BSDFQueryRecord& bRec, const Vector2f& u, float& pdf) const override;

  std::string toString() const override;

private:
  std::string filename;
  bool remapRoughness;
  float uRoughness;
  float vRoughness;
  Spectrum kr;
  Spectrum eta, k;
  TrowbridgeReitzDistribution distrib;
};

}
