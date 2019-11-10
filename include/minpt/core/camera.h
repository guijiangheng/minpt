#pragma once

#include <minpt/math/math.h>
#include <minpt/core/filter.h>

namespace minpt {

struct CameraSample {
  Vector2f pFilm;
};

class Camera : public Object {
public:
  Camera(const Matrix4f& frame, const Vector2i& outputSize) noexcept
    : frame(frame), outputSize(outputSize), filter(nullptr)
  { }

  ~Camera() {
    delete filter;
  }

  void addChild(Object* child) override {
    switch (child->getClassType()) {
    case EFilter:
      if (filter)
        throw Exception("There can only be one filter per camera!");
      filter = static_cast<Filter*>(child);
      break;
    default:
      throw Exception("Camera::addChild(<%s>) is not supported", classTypeName(child->getClassType()));
    }
  }

  void activate() override {
    if (!filter)
      filter = static_cast<Filter*>(ObjectFactory::createInstance("gaussian", PropertyList()));
  }

  virtual Ray generateRay(const CameraSample& sample) const = 0;

  EClassType getClassType() const override {
    return ECamera;
  }

public:
  Matrix4f frame;
  Vector2i outputSize;
  Filter* filter;
};

}
