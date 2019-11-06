#pragma once

#include <minpt/core/ray.h>
#include <minpt/core/sampler.h>

namespace minpt {

class Scene;

class Integrator : public Object {
public:
  virtual void preprocess(const Scene& scene) { }

  virtual Color3f li(const Ray& ray, const Scene&, Sampler& sampler) const = 0;

  EClassType getClassType() const override {
    return EIntegrator;
  }
};

}
