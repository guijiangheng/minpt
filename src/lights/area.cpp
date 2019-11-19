#include <minpt/core/mesh.h>
#include <minpt/core/light.h>
#include <minpt/core/visibilitytester.h>

namespace minpt {

class AreaLight : public Light {
public:
  AreaLight(const PropertyList& props)
    : mesh(nullptr)
    , radiance(props.getColor3f("radiance"))
  { }

  Color3f le(const LightSample& pLight, const Vector3f& wo) const {
    if (twoSided || dot(pLight.p, wo) > 0)
      return radiance;
    return Color3f(0.0f);
  }

  Color3f sample(
      const Interaction& ref,
      const Vector2f& u,
      Vector3f& wi,
      float& pdf,
      VisibilityTester& tester) const override {

    auto sample = u;
    auto pLight = mesh->sample(ref.p, sample, pdf);
    wi = normalize(pLight.p - ref.p);
    tester = VisibilityTester(ref, pLight.p);
    return le(pLight, -wi);
  }

  std::string toString() const override {
    return tfm::format(
      "AreaLight[\n"
      "  twoSided = %s,\n"
      "  radiance = %s\n"
      "]",
      twoSided ? "true" : "false", radiance.toString()
    );
  }

public:
  Mesh* mesh;
  Color3f radiance;
  bool twoSided;
};

MINPT_REGISTER_CLASS(AreaLight, "area");

}
