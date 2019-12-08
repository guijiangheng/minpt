#pragma once

#include <minpt/utils/utils.h>
#include <minpt/math/math.h>
#include <minpt/core/bsdf.h>
#include <minpt/core/sampling.h>

namespace minpt {

class Diffuse : public BSDF {
public:
  Diffuse(const PropertyList& props)
    : albedo(props.getColor3f("albedo", Color3f(0.0f)))
  { }

  Color3f f(const Vector3f& wo, const Vector3f& wi) const override {
    if (!sameHemisphere(wo, wi))
      return Color3f(0.0f);
    return albedo * InvPi;
  }

  float pdf(const Vector3f& wo, const Vector3f& wi) const override {
    return sameHemisphere(wo, wi) ? absCosTheta(wi) * InvPi : 0;
  }

  Color3f sample(
      const Vector2f& u,
      const Vector3f& wo,
      Vector3f& wi,
      float& pdf,
      float& etaScale) const override {

    etaScale = 1.0f;
    wi = cosineSampleHemisphere(u);
    pdf = wi.z * InvPi;
    if (wo.z < 0.0f) wi.z = -wi.z;
    return albedo;
  }

  std::string toString() const override {
    return tfm::format("Diffuse[albedo = %s]", albedo.toString());
  }

private:
  Color3f albedo;
};

}
