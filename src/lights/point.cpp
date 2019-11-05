#include <minpt/core/light.h>
#include <minpt/core/visibilitytester.h>

namespace minpt {

class PointLight : public Light {
public:
  PointLight(const PropertyList& props)
    : pLight(props.getVector3f("pLight"))
    , intensity(props.getColor3f("intensity"))
  { }

  bool isDelta() const override {
    return true;
  }

  Color3f sample(
      const Interaction& ref,
      const Vector2f& u,
      Vector3f& wi,
      float& pdf,
      VisibilityTester& tester) const override {

    pdf = 1.0f;
    auto d = pLight - ref.p;
    wi = normalize(d);
    tester = VisibilityTester(ref, pLight);
    return intensity / d.lengthSquared();
  }

  std::string toString() const override {
    return tfm::format(
      "PointLight[\n"
      "  pLight = %s,\n"
      "  intensity = %s\n"
      "]",
      pLight.toString(), intensity.toString()
    );
  }

public:
  Vector3f pLight;
  Color3f intensity;
};

MINPT_REGISTER_CLASS(PointLight, "point");

}
