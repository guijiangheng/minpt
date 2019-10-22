#include <minpt/core/scene.h>
#include <minpt/core/sampling.h>

namespace minpt {

class AmbientOcclusionIntegrator : public Integrator {
public:
  AmbientOcclusionIntegrator(const PropertyList& props)
    : samples(props.getInteger("samples", 32))
  { }

  Color3f li(const Ray& ray, const Scene& scene, Sampler& sampler) const override {
    Interaction isect;
    Color3f ret(0.0f);
    if (!scene.intersect(ray, isect))
      return ret;
    Frame frame(isect.n);
    for (auto i = 0; i < samples; ++i) {
      auto p = cosineSampleHemisphere(sampler.get2D());
      auto w = frame.toWorld(p);
      auto shadowRay = isect.spawnRay(w);
      shadowRay.tMax = 1.0f;
      ret += scene.intersect(shadowRay) ? Color3f(0) : Color3f(1);
    }
    return ret / samples;
  }

  std::string toString() const override {
    return tfm::format("AmbientOcclusionIntegrator[samples=%d]", samples);
  }

private:
  int samples;
};

MINPT_REGISTER_CLASS(AmbientOcclusionIntegrator, "ao");

}
