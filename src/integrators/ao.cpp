#include <minpt/core/scene.h>
#include <minpt/core/sampling.h>

namespace minpt {

class AmbientOcclusionIntegrator : public Integrator {
public:
  AmbientOcclusionIntegrator(const PropertyList& props)
    : shadingSamples(props.getInteger("shadingSamples", 1))
    , rayLength(props.getFloat("rayLength", -1.0f))
  { }

  void preprocess(const Scene& scene) override {
    if (rayLength < 0)
      rayLength = scene.getBoundingBox().pMin.length() * 0.5f;
  }

  Color3f li(const Ray& ray, const Scene& scene, Sampler& sampler) const override {
    Interaction isect;
    Color3f ret(0.0f);
    if (!scene.intersect(ray, isect))
      return ret;
    for (auto i = 0; i < shadingSamples; ++i) {
      auto w = isect.toWorld(cosineSampleHemisphere(sampler.get2D()));
      auto shadowRay = isect.spawnRay(w, rayLength);
      if (!scene.intersect(shadowRay))
        ret += Color3f(1.0f);
    }
    return ret / shadingSamples;
  }

  std::string toString() const override {
    return tfm::format(
      "AmbientOcclusionIntegrator[\n"
      "  shadingSamples = %d,\n"
      "  rayLength = %f\n"
      "]",
      shadingSamples, rayLength
    );
  }

private:
  int shadingSamples;
  float rayLength;
};

MINPT_REGISTER_CLASS(AmbientOcclusionIntegrator, "ao");

}
