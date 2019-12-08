#pragma once

#include <minpt/core/microfact.h>

namespace minpt {

class BeckmannDistribution : public MicrofacetDistribution {
public:
  BeckmannDistribution() noexcept = default;

  BeckmannDistribution(float alphaX, float alphaY) : alphaX(alphaX), alphaY(alphaY)
  { }

  static float roughnessToAlpha(float roughness) {
    roughness = std::max(roughness, 0.001f);
    auto x = std::log(roughness);
    auto x2 = x * x;
    return 1.62142f + 0.819955f * x + 0.1734f * x2 +
           0.0171201f * x * x2 + 0.000640711f * x2* x2;
  }

  float lambda(const Vector3f& w) const override;

  float d(const Vector3f& wh) const override;

  Vector3f sample(const Vector2f& u) const override;

  std::string toString() const override {
    return tfm::format("BeckmannDistribution[alphaX = %f, alphaY = %f]", alphaX, alphaY);
  }

public:
  float alphaX, alphaY;
};

}
