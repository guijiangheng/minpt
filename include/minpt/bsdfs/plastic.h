#pragma once

#include <minpt/core/bsdf.h>
#include <minpt/microfacets/beckmann.h>

namespace minpt {

class Plastic : public BSDF {
public:
  Plastic(const PropertyList& props);

  static float fr(float cosThetaI, float eta);

  Color3f f(const Vector3f& wo, const Vector3f& wi) const override;

  float pdf(const Vector3f& wo, const Vector3f& wi) const override {
    if (!sameHemisphere(wo, wi))
      return 0.0f;
    auto wh = normalize(wo + wi);
    return ks * distrib.pdf(wh) / (4.0f * dot(wh, wo)) + (1.0f - ks) * absCosTheta(wi) * InvPi;
  }

  Color3f sample(
    const Vector2f& u,
    const Vector3f& wo,
    Vector3f& wi,
    float& pdf,
    float& etaScale) const override;

  std::string toString() const override;

private:
  bool remapRoughness;
  float roughness;
  float eta;
  Color3f kd;
  float ks;
  BeckmannDistribution distrib;
};

}
