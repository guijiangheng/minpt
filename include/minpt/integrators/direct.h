#pragma once

#include <minpt/core/scene.h>
#include <minpt/lights/area.h>

namespace minpt {

class DirectIntegrator : public Integrator {
public:
  DirectIntegrator(const PropertyList& props)
  { }

  Color3f estimateDirect(
      const Interaction& isect,
      const Scene& scene,
      const Light& light,
      Sampler& sampler) const {

    float pdf;
    Vector3f wi;
    VisibilityTester tester;
    auto li = light.sample(isect, sampler.get2D(), wi, pdf, tester);

    if (li.isBlack())
      return Color3f(0.0f);

    auto wiLocal = isect.toLocal(wi);
    auto woLocal = isect.toLocal(isect.wo);
    auto f = isect.mesh->bsdf->f(woLocal, wiLocal);

    if (f.isBlack() || tester.occluded(scene))
      return Color3f(0);

    return f * li * absCosTheta(wiLocal) / pdf;
  }

  Color3f sampleOneLight(const Interaction& isect, const Scene& scene, Sampler& sampler) const {
    auto nLights = scene.lights.size();
    if (!nLights) return Color3f(0.0f);
    auto index = std::min((std::size_t)(sampler.get1D() * nLights), nLights - 1);
    return estimateDirect(isect, scene, *scene.lights[index], sampler) * nLights;
  }

  Color3f li(const Ray& ray, const Scene& scene, Sampler& sampler) const override {
    Interaction isect;
    if (!scene.intersect(ray, isect))
      return Color3f(0.0f);
    if (isect.mesh->light)
      return isect.mesh->light->le(isect);
    return sampleOneLight(isect, scene, sampler);
  }

  std::string toString() const override {
    return "DirectIntegrator[]";
  }
};

}
