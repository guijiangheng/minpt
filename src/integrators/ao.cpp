#include <minpt/core/scene.h>
#include <minpt/core/sampling.h>

namespace minpt {

class AmbientOcclusionIntegrator : public Integrator {
public:
  AmbientOcclusionIntegrator(const PropertyList& props)
  { }

  Color3f li(const Ray& ray, const Scene& scene, Sampler& sampler) const override {
    Interaction isect;
    if (!scene.intersect(ray, isect))
      return Color3f(0.0f);
    auto p = uniformSampleHemisphere(sampler.get2D());
    auto w = Frame(isect.n).toWorld(p);
    auto shadowRay = isect.spawnRay(w);
    shadowRay.tMax = 1.0f;
    return scene.intersect(shadowRay) ? Color3f(0.0f) : Color3f(1.0f);

    Frame frame(isect.n);
  }

  std::string toString() const override {
    return "AmbientOcclusionIntegrator[]";
  }
};

MINPT_REGISTER_CLASS(AmbientOcclusionIntegrator, "ao");

}
