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

template <>
ConstantTexture<float>::ConstantTexture(const PropertyList& props)
  : value(props.getFloat("value", 0.0f))
{ }

template <>
ConstantTexture<Spectrum>::ConstantTexture(const PropertyList& props)
  : value(props.getRGBSpectrum("value", Spectrum(0.0f)))
{ }

template <>
std::string ConstantTexture<float>::toString() const {
  return tfm::format("ConstantTexture[value = %f]", value);
}

template <>
std::string ConstantTexture<Spectrum>::toString() const {
  return tfm::format("ConstantTexture[value = %s]", value.toString());
}

}
