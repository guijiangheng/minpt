#pragma once

#include <string>
#include <minpt/core/exception.h>

namespace minpt {

class Object {
public:
  enum EClassType {
    EScene = 0,
    EAccel,
    EMesh,
    ECamera,
    EIntegrator,
    ESampler,
    EClassTypeCount
  };

  virtual ~Object() = default;

  virtual void addChild(Object* child) {
    throw Exception(
      "Object::addChild() is not implemented for objects of type '%s'!",
      classTypeName(getClassType())
    );
  }

  virtual void activate()
  { }

  virtual EClassType getClassType() const = 0;

  virtual std::string toString() const = 0;

  static std::string classTypeName(EClassType type) {
    switch (type) {
      case EScene:      return "scene";
      case EAccel:      return "accel";
      case EMesh:       return "mesh";
      case ECamera:     return "camera";
      case EIntegrator: return "integrator";
      case ESampler:    return "sampler";
      default:          return "<unknown>";
    }
  }
};

}
