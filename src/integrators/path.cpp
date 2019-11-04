#include <minpt/core/scene.h>

namespace minpt {

class PathIntegrator : public Integrator {
public:
  PathIntegrator(const PropertyList& props)
  { }

  Color3f li(const Ray& ray, const Scene& scene, Sampler& sampler) const override {
    return Color3f(0);
  }

  std::string toString() const override {
    return "PathIntegrator[]";
  }
};

MINPT_REGISTER_CLASS(PathIntegrator, "path");

}
