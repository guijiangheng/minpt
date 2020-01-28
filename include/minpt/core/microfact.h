#pragma once

#include <minpt/math/math.h>

namespace minpt {

class MicrofacetDistribution {
public:
  MicrofacetDistribution() noexcept = default;

  MicrofacetDistribution(float alphaU, float alphaV) : alphaU(alphaU), alphaV(alphaV)
  { }

  virtual ~MicrofacetDistribution() = default;

  bool isIsotropic() const {
    return alphaU == alphaV;
  }

  float projectRoughness(const Vector3f& w) const {
    auto invSinTheta2 = 1.0f / sin2Theta(w);

    if (isIsotropic() || invSinTheta2 <= 0)
      return alphaU;

    auto cosPhi2 = w.x * w.x * invSinTheta2;
    auto sinPhi2 = w.y * w.y * invSinTheta2;
    return std::sqrt(cosPhi2 * alphaU * alphaU + sinPhi2 * alphaV * alphaV);
  }

  float g(const Vector3f& wo, const Vector3f& wi, const Vector3f& wh) const {
    return smithG1(wo, wh) * smithG1(wi, wh);
  }

  float pdf(const Vector3f& wh) const {
    return d(wh) * cosTheta(wh);
  }

  virtual float smithG1(const Vector3f& w, const Vector3f& wh) const = 0;

  virtual float d(const Vector3f& wh) const = 0;

  virtual Vector3f sample(const Vector2f& u, float* pdf = nullptr) const = 0;

  virtual std::string toString() const = 0;

public:
  float alphaU, alphaV;
};

}
