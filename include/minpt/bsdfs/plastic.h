#pragma once

#include <minpt/core/bsdf.h>
#include <minpt/microfacets/trowbridge.h>

namespace minpt {

class Plastic : public BSDF {
public:
  Plastic(const PropertyList& props);

  Color3f f(const BSDFQueryRecord& bRec) const override;

  float pdf(const BSDFQueryRecord& bRec) const override {
    if (!sameHemisphere(bRec.wo, bRec.wi))
      return 0.0f;
    auto wh = normalize(bRec.wo + bRec.wi);
    if (wh.z < 0) wh = -wh;
    return ks * distrib.pdf(wh) / (4.0f * absdot(wh, bRec.wo)) + (1.0f - ks) * absCosTheta(bRec.wi) * InvPi;
  }

  Color3f sample(BSDFQueryRecord& bRec, const Vector2f& u, float& pdf) const override;

  std::string toString() const override;

private:
  bool remapRoughness;
  float roughness;
  float eta;
  Color3f kd;
  float ks;
  TrowbridgeReitzDistribution distrib;
};

}
