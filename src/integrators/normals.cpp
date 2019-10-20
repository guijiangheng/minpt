#include <minpt/core/scene.h>

namespace minpt {

class NormalIntegrator : public Integrator {
public:
  NormalIntegrator(const PropertyList& props)
  { }

  Color3f li(const Ray& ray, const Scene& scene, Sampler& sampler) const override {
    Interaction isect;
    if (scene.intersect(ray, isect)) {
      return Color3f(abs(isect.shFrame.n));
    }
    return Color3f(0.0f);
  }

  std::string toString() const override {
    return "NormalIntegrator[]";
  }

};

MINPT_REGISTER_CLASS(NormalIntegrator, "normals");

}
