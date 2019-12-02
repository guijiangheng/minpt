#pragma once

#include <minpt/math/math.h>

namespace minpt {

class MicrofacetDistribution {
public:
  virtual ~MicrofacetDistribution() = default;

  float pdf(const Vector3f& wh) const {
    return d(wh) * absCosTheta(wh);
  }

  virtual float d(const Vector3f& wh) const = 0;

  virtual Vector3f sample(const Vector2f& u) const = 0;

  virtual std::string toString() const = 0;
};

}
