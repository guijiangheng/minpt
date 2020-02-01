#pragma once

#include <any>
#include <map>
#include <minpt/math/math.h>
#include <minpt/core/exception.h>
#include <minpt/core/spectrum.h>

namespace minpt {

#define DEFINE_PROPERTY_ACCESSOR(Type, TypeName)                                              \
  void set##TypeName(const std::string& name, const Type& value) {                            \
    if (has(name))                                                                            \
      std::cerr << "Property \""  << name << "\" was specified multiple times!" << std::endl; \
    props[name] = value;                                                                      \
  }                                                                                           \
                                                                                              \
  Type get##TypeName(const std::string& name) const {                                         \
    auto it = props.find(name);                                                               \
    if (it == props.end())                                                                    \
      throw Exception("Property '%s' is missing!", name);                                     \
    return std::any_cast<Type>(it->second);                                                   \
  }                                                                                           \
                                                                                              \
  Type get##TypeName(const std::string& name, const Type& defaultValue) const {               \
    auto it = props.find(name);                                                               \
    if (it == props.end())                                                                    \
      return defaultValue;                                                                    \
    return std::any_cast<Type>(it->second);                                                   \
  }


class PropertyList {
public:
  PropertyList() = default;

  bool has(const std::string& name) const {
    return props.find(name) != props.end();
  }

  DEFINE_PROPERTY_ACCESSOR(bool, Boolean);
  DEFINE_PROPERTY_ACCESSOR(int, Integer);
  DEFINE_PROPERTY_ACCESSOR(float, Float);
  DEFINE_PROPERTY_ACCESSOR(std::string, String);
  DEFINE_PROPERTY_ACCESSOR(Vector2i, Vector2i);
  DEFINE_PROPERTY_ACCESSOR(Vector2f, Vector2f);
  DEFINE_PROPERTY_ACCESSOR(RGBSpectrum, RGBSpectrum);
  DEFINE_PROPERTY_ACCESSOR(Vector3f, Vector3f);
  DEFINE_PROPERTY_ACCESSOR(Bounds2f, Bounds2f);
  DEFINE_PROPERTY_ACCESSOR(Matrix4f, Transform);

private:
  std::map<std::string, std::any> props;
};

}
