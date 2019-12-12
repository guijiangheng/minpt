#pragma once

#include <minpt/core/object.h>

namespace minpt {

template <typename T>
class Texture : public Object {
public:
  virtual ~Texture() = default;

  virtual EClassType getClassType() const override {
    return ETexture;
  }

  virtual T eval(const Vector2f& uv) const = 0;
};

}
