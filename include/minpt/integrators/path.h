#pragma once

#include <minpt/core/scene.h>

namespace minpt {

class PathIntegrator : public Integrator {
public:
  PathIntegrator(const PropertyList& props) : maxDepth(props.getInteger("maxDepth", 3))
  { }

  Color3f li(const Ray& ray, const Scene& scene, Sampler& sampler) const override;

  std::string toString() const override {
    return tfm::format("PathIntegrator[maxDepth = %d]", maxDepth);
  }

public:
  int maxDepth;
};

}
