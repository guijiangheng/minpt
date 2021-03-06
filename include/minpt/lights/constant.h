#pragma once

#include <minpt/core/scene.h>
#include <minpt/core/light.h>
#include <minpt/core/visibilitytester.h>

namespace minpt {

class ConstantEnvLight : public InfiniteLight {
public:
  ConstantEnvLight(const PropertyList& props)
    : radiance(props.getRGBSpectrum("radiance", Spectrum(1.0f)))
  { }

  void preprocess(const Scene& scene) override {
    worldDiameter = scene.getBoundingBox().diag().length();
  }

  Spectrum le(const Ray& ray) const override {
    return radiance;
  }

  Spectrum sample(
      const Interaction& ref,
      const Vector2f& u,
      Vector3f& wi,
      float& pdf,
      VisibilityTester& tester) const override {

    auto wiLocal = uniformSampleSphere(u);
    pdf = Inv4Pi;
    wi = ref.toWorld(wiLocal);
    tester = VisibilityTester(ref, ref.p + wi * worldDiameter);
    return radiance;
  }

  float pdf(const Vector3f& w) const override {
    return Inv4Pi;
  }

  std::string toString() const override {
    return tfm::format("ConstantEnvLight[radiance = %s]", radiance.toString());
  }

private:
  Spectrum radiance;
  float worldDiameter;
};

}
