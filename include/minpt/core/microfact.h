#pragma once

#include <minpt/core/object.h>

namespace minpt {

class MicrofacetDistribution : public Object {
public:
  virtual Vector3f sample(const Vector2f& u) const = 0;

  EClassType getClassType() const override {
    return EMicrofacet;
  }
};

}
