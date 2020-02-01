#pragma once

#include <minpt/core/texture.h>

namespace minpt {

template <typename T>
class CheckerboardTexture : public Texture<T> {
public:
  CheckerboardTexture(const PropertyList& props);

  T eval(const Vector2f& uv) const override {
    auto value = Vector2i(floor(mapping.map(uv)));
    return ((value.x % 2) ^ (value.y % 2)) ? value2 : value1;
  }

  std::string toString() const override;

public:
  T value1;
  T value2;
  TextureMapping2D mapping;
};

template <>
CheckerboardTexture<float>::CheckerboardTexture(const PropertyList& props)
  : value1(props.getFloat("value1", 0.0f))
  , value2(props.getFloat("value2", 1.0f))
  , mapping(
      props.getVector2f("scale", Vector2f(1.0f)),
      props.getVector2f("delta", Vector2f(0.0f)))
{ }

template <>
CheckerboardTexture<Spectrum>::CheckerboardTexture(const PropertyList& props)
  : value1(props.getRGBSpectrum("value1", Spectrum(0.0f)))
  , value2(props.getRGBSpectrum("value2", Spectrum(1.0f)))
  , mapping(
        props.getVector2f("scale", Vector2f(1.0f)),
        props.getVector2f("delta", Vector2f(0.0f)))
{ }

template <>
std::string CheckerboardTexture<float>::toString() const {
  return tfm::format(
    "CheckboardTexture[\n"
    "  value1 = %f,\n"
    "  value2 = %f,\n"
    "  mapping = %s\n"
    "]",
    value1, value2, indent(mapping.toString())
  );
}

template <>
std::string CheckerboardTexture<Spectrum>::toString() const {
  return tfm::format(
    "CheckboardTexture[\n"
    "  value1 = %s,\n"
    "  value2 = %s,\n"
    "  mapping = %s\n"
    "]",
    value1.toString(), value2.toString(), indent(mapping.toString())
  );
}

}
