#pragma once

#include <minpt/core/ray.h>
#include <minpt/math/color.h>
#include <minpt/core/sampler.h>

namespace minpt {

class Scene;

class Integrator {
public:
  virtual ~Integrator() = default;

  virtual Color3f li(const Ray3f& ray, const Scene&, Sampler& sampler) const = 0;

  virtual std::string toString() const = 0;
};

}
