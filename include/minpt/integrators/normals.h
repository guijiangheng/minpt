#pragma once

#include <minpt/core/scene.h>
#include <minpt/core/integrator.h>
#include <minpt/core/interaction.h>

namespace minpt {

class NormalIntegrator : public Integrator {
public:
  Color3f li(const Ray3f& ray, const Scene& scene, Sampler& sampler) const override {
    Interaction isect;
    if (scene.intersect(ray, isect)) {
      auto n = isect.shFrame.n.cwiseAbs();
      return Color3f(n.x(), n.y(), n.z());
    }
    return Color3f(0.0f);
  }

  std::string toString() const override {
    return "NormalIntegrator[]";
  }

};

}
