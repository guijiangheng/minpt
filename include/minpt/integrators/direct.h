#pragma once

#include <minpt/core/scene.h>
#include <minpt/lights/area.h>

namespace minpt {

class DirectIntegrator : public Integrator {
public:
  DirectIntegrator(const PropertyList& props)
  { }

  const Light& getOneLight(const Scene& scene, Sampler& sampler, float& pdf) const {
    auto nLights = scene.lights.size();
    pdf = 1.0f / nLights;
    auto index = std::min((std::size_t)(sampler.get1D() * nLights), nLights - 1);
    return *scene.lights[index];
  }

  Spectrum li(const Ray& ray, const Scene& scene, Sampler& sampler) const override;

  std::string toString() const override {
    return "DirectIntegrator[]";
  }
};

}
