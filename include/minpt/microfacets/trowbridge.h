#pragma once

#include <minpt/core/microfact.h>

namespace minpt {

class TrowbridgeReitzDistribution : public MicrofacetDistribution {
public:
  static float roughnessToAlpha(float roughness) {
    roughness = std::max(roughness, 0.001f);
    auto x = std::log(roughness);
    auto x2 = x * x;
    return 1.62142f + 0.819955f * x + 0.1734f * x2 + 0.0171201f * x2 * x +
           0.000640711f * x2 * x2;
  }

  TrowbridgeReitzDistribution() noexcept = default;

  TrowbridgeReitzDistribution(float alphaU, float alphaV)
    : MicrofacetDistribution(alphaU, alphaV)
  { }

  float smithG1(const Vector3f& w, const Vector3f& wh) const override;

  float d(const Vector3f& wh) const override;

  Vector3f sample(const Vector2f& u, float* pdf = nullptr) const override;

  std::string toString() const override {
    return tfm::format("TrowbridgeReitzDistribution[alphaU = %f, alphaV = %f]", alphaU, alphaV);
  }
};

}
