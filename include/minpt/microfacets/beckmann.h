#pragma once

#include <minpt/core/microfact.h>

namespace minpt {

class BeckmannDistribution : public MicrofacetDistribution {
public:
  BeckmannDistribution(float alphaX, float alphaY) : alphaX(alphaX), alphaY(alphaY)
  { }

  static float roughnessToAlpha(float roughness) {
    roughness = std::max(roughness, 0.001f);
    auto x = std::log(roughness);
    return 1.62142f + 0.819955f * x + 0.1734f * x * x +
           0.0171201f * x * x * x + 0.000640711f * x * x * x * x;
  }

  float d(const Vector3f& wh) const override;

  Vector3f sample(const Vector2f& u) const override;

  std::string toString() const override {
    return tfm::format("BeckmannDistribution[alphaX = %f, alphaY = %f]", alphaX, alphaY);
  }

private:
  float alphaX, alphaY;
};

}
