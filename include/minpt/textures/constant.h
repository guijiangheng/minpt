#pragma once

#include <minpt/core/texture.h>

namespace minpt {

template <typename T>
class ConstantTexture : public Texture<T> {
public:
  ConstantTexture(const PropertyList& props);

  std::string toString() const override;

  T eval(const Vector2f& uv) const override {
    return value;
  }

public:
  T value;
};

template<>
ConstantTexture<float>::ConstantTexture(const PropertyList& props)
  : value(props.getFloat("value", 0.0f))
{ }

template<>
ConstantTexture<Color3f>::ConstantTexture(const PropertyList& props)
  : value(props.getColor3f("value", Color3f(0.0f)))
{ }

template<>
std::string ConstantTexture<float>::toString() const {
  return tfm::format("ConstantTexture[value = %f]", value);
}

template<>
std::string ConstantTexture<Color3f>::toString() const {
  return tfm::format("ConstantTexture[value = %s]", value.toString());
}

}
