#pragma once

#include <any>
#include <map>

#include <minpt/math/color3.h>
#include <minpt/math/bounds2.h>
#include <minpt/math/matrix4.h>
#include <minpt/core/exception.h>

namespace minpt {

#define DEFINE_PROPERTY_ACCESSOR(Type, TypeName)                                              \
  void set##TypeName(const std::string& name, const Type& value) {                            \
    if (props.find(name) != props.end())                                                      \
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

  DEFINE_PROPERTY_ACCESSOR(bool, Boolean);
  DEFINE_PROPERTY_ACCESSOR(int, Integer);
  DEFINE_PROPERTY_ACCESSOR(float, Float);
  DEFINE_PROPERTY_ACCESSOR(std::string, String);
  DEFINE_PROPERTY_ACCESSOR(Vector2i, Vector2i);
  DEFINE_PROPERTY_ACCESSOR(Color3f, Color3f);
  DEFINE_PROPERTY_ACCESSOR(Vector3f, Vector3f);
  DEFINE_PROPERTY_ACCESSOR(Bounds2f, Bounds2f);
  DEFINE_PROPERTY_ACCESSOR(Matrix4f, Transform);

private:
  std::map<std::string, std::any> props;
};

}
