#pragma once

#include <minpt/core/scene.h>

namespace minpt {

class NormalIntegrator : public Integrator {
public:
  NormalIntegrator(const PropertyList& props)
  { }

  Spectrum li(const Ray& ray, const Scene& scene, Sampler& sampler) const override {
    Interaction isect;
    if (scene.intersect(ray, isect)) {
      auto n = abs(isect.shFrame.n);
      return Spectrum(n.x, n.y, n.z);
    }
    return Spectrum(0.0f);
  }

  std::string toString() const override {
    return "NormalIntegrator[]";
  }
};

}
