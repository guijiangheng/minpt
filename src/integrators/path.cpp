#include <minpt/core/scene.h>

namespace minpt {

class PathIntegrator : public Integrator {
public:
  PathIntegrator(const PropertyList& props)
  { }

  Color3f li(const Ray& ray, const Scene& scene, Sampler& sampler) const override {
    Interaction isect;
    if (!scene.intersect(ray, isect))
      return Color3f(0.0f);

    float pdf;
    Vector3f wi;
    VisibilityTester tester;
    auto li = scene.lights[0]->sample(isect, sampler.get2D(), wi, pdf, tester);

    auto reflect = dot(isect.n, isect.wo) * dot(isect.n, wi) > 0.0f;
    auto wiLocal = isect.shFrame.toLocal(wi);
    auto woLocal = isect.shFrame.toLocal(isect.wo);

    if (li.isBlack() || !reflect || !sameHemisphere(wiLocal, woLocal) || tester.occluded(scene))
      return Color3f(0.0f);

    return li * dot(wi, isect.shFrame.n) / pdf;
  }

  std::string toString() const override {
    return "PathIntegrator[]";
  }
};

MINPT_REGISTER_CLASS(PathIntegrator, "path");

}