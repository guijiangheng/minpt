#pragma once

#include <minpt/core/mesh.h>
#include <minpt/core/light.h>
#include <minpt/core/visibilitytester.h>

namespace minpt {

class AreaLight : public Light {
public:
  AreaLight(const PropertyList& props)
    : mesh(nullptr)
    , radiance(props.getRGBSpectrum("radiance"))
    , twoSided(props.getBoolean("twoSided", true))
  { }

  Spectrum le(const Interaction& isect, const Vector3f& wo) const {
    if (twoSided || dot(isect.n, wo) > 0)
      return radiance;
    return Spectrum(0.0f);
  }

  Spectrum le(const LightSample& pLight, const Vector3f& wo) const {
    if (twoSided || dot(pLight.n, wo) > 0)
      return radiance;
    return Spectrum(0.0f);
  }

  Spectrum sample(
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

  float pdf(const Vector3f& ref, const Interaction& pLight) const {
    auto p = 1.0f / mesh->getTotalArea();
    auto d = pLight.p - ref;
    auto w = normalize(d);
    p *= d.lengthSquared() / absdot(pLight.n, w);
    return p;
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
  Spectrum radiance;
  bool twoSided;
};

}
