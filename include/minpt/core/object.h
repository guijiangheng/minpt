#pragma once

#include <string>

namespace minpt {

class Object {
public:
  enum EClassType {
    EScene = 0,
    EMesh,
    ECamera,
    EIntegrator,
    ESampler,
    EClassTypeCount
  };

  virtual EClassType getClassType() const = 0;

  virtual std::string toString() const = 0;

  static std::string classTypeName(EClassType type) {
    switch (type) {
      case EScene:      return "scene";
      case EMesh:       return "mesh";
      case ECamera:     return "camera";
      case EIntegrator: return "integrator";
      case ESampler:    return "sampler";
      default:          return "<unknown>";
    }
  }

protected:
  virtual ~Object() = default;
};

}
