#pragma once

#include <minpt/core/ray.h>
#include <minpt/core/sampler.h>

namespace minpt {

class Scene;

class Integrator : public Object {
public:
  virtual void preprocess(const Scene& scene)
  { }

  virtual Spectrum li(const Ray& ray, const Scene&, Sampler& sampler) const = 0;

  static float weight(float a, float b) {
    return a / (a + b);
  }

  EClassType getClassType() const override {
    return EIntegrator;
  }
};

}
