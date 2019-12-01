#pragma once

#include <minpt/core/microfact.h>

namespace minpt {

class BeckmannDistribution : public MicrofacetDistribution {
public:
  BeckmannDistribution(const PropertyList& props)
    : alphaX(props.getFloat("alphaX"))
    , alphaY(props.getFloat("alphaY"))
  { }

  Vector3f sample(const Vector2f& u) const override;

  std::string toString() const override {
    return tfm::format("BeckmannDistribution[alphaX = %f, alphaY = %f]", alphaX, alphaY);
  }

private:
  float alphaX, alphaY;
};

}
