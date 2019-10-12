#pragma once

#include <map>
#include <any>
#include <iostream>

#include <minpt/math/color.h>
#include <minpt/math/matrix.h>
#include <minpt/core/exception.h>

namespace minpt {
class PropertyList {
public:
  PropertyList() = default;

  void setBoolean(const std::string& name, bool value) {
    warnDuplicate(name);
    props[name] = value;
  }

  bool getBoolean(const std::string& name) const {
    checkExist(name);
    return std::any_cast<bool>(props.at(name));
  }

  bool getBoolean(const std::string& name, bool defaultValue) const {
    auto it = props.find(name);
    if (it == props.end()) return defaultValue;
    return std::any_cast<bool>(it->second);
  }

  void setInteger(const std::string& name, int value) {
    warnDuplicate(name);
    props[name] = value;
  }

  int getInteger(const std::string& name) const {
    checkExist(name);
    return std::any_cast<int>(props.at(name));
  }

  int getInteger(const std::string& name, int defaultValue) const {
    auto it = props.find(name);
    if (it == props.end()) return defaultValue;
    return std::any_cast<int>(it->second);
  }

  void setFloat(const std::string& name, float value) {
    warnDuplicate(name);
    props[name] = value;
  }

  float getFloat(const std::string& name) const {
    checkExist(name);
    return std::any_cast<float>(props.at(name));
  }

  float getFloat(const std::string& name, float defaultValue) const {
    auto it = props.find(name);
    if (it == props.end()) return defaultValue;
    return std::any_cast<float>(it->second);
  }

  void setString(const std::string& name, const std::string& value) {
    warnDuplicate(name);
    props[name] = value;
  }

  std::string getString(const std::string& name) const {
    checkExist(name);
    return std::any_cast<std::string>(props.at(name));
  }

  std::string getString(const std::string& name, const std::string& defaultValue) const {
    auto it = props.find(name);
    if (it == props.end()) return defaultValue;
    return std::any_cast<std::string>(it->second);
  }

  void setColor(const std::string& name, const Color3f& value) {
    warnDuplicate(name);
    props[name] = value;
  }

  Color3f getColor(const std::string& name) const {
    checkExist(name);
    return std::any_cast<Color3f>(props.at(name));
  }

  Color3f getColor(const std::string& name, const Color3f& defaultValue) const {
    auto it = props.find(name);
    if (it == props.end()) return defaultValue;
    return std::any_cast<Color3f>(it->second);
  }

  void setVector(const std::string& name, const Vector3f& value) {
    warnDuplicate(name);
    props[name] = value;
  }

  Vector3f getVector(const std::string& name) const {
    checkExist(name);
    return std::any_cast<Vector3f>(props.at(name));
  }

  Vector3f getVector(const std::string& name, const Vector3f& defaultValue) const {
    auto it = props.find(name);
    if (it == props.end()) return defaultValue;
    return std::any_cast<Vector3f>(it->second);
  }

  void setTransform(const std::string& name, const Matrix4f& value) {
    warnDuplicate(name);
    props[name] = value;
  }

  Matrix4f getTransform(const std::string& name) const {
    checkExist(name);
    return std::any_cast<Matrix4f>(props.at(name));
  }

  Matrix4f getTransform(const std::string& name, const Matrix4f& defaultValue) const {
    auto it = props.find(name);
    if (it == props.end()) return defaultValue;
    return std::any_cast<Matrix4f>(it->second);
  }

private:
  void warnDuplicate(const std::string& name) const {
    if (props.find(name) != props.end())
      std::cerr << "Property \""  << name << "\" was specified multiple times!" << std::endl;
  }

  void checkExist(const std::string& name) const {
    if (props.find(name) == props.end())
      throw Exception("Property '%s' is missing!", name);
  }

private:
  std::map<std::string, std::any> props;
};

}
