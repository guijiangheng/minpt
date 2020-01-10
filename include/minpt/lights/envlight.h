#pragma once

#include <minpt/core/scene.h>
#include <minpt/core/light.h>
#include <minpt/core/visibilitytester.h>
#include <minpt/core/distribution.h>
#include <minpt/textures/image.h>

namespace minpt {

class EnvironmentLight : public InfiniteLight {
public:
  EnvironmentLight(const PropertyList& props)
    : envMap(nullptr)
    , toWorld(props.getTransform("toWorld", Matrix4f::identity()))
    , toLocal(toWorld.inverse())
  { }

  ~EnvironmentLight() {
    delete envMap;
  }

  void addChild(Object* child) override {
    switch (child->getClassType()) {
      case ETexture:
        if (envMap)
          throw Exception("There is already an envMap!");
        envMap = static_cast<ImageTexture<Color3f>*>(child);
        break;
      default:
        throw Exception(
          "EnvironmentLight::addChild() is not implemented for objects of type '%s'!",
          classTypeName(child->getClassType())
        );
    }
  }

  void activate() override {
    if (!envMap)
      throw Exception("No envMap was specified!");
    auto width = envMap->width;
    auto height = envMap->height;
    auto data = std::make_unique<float[]>(width * height);
    for (auto y = 0; y < height; ++y) {
      auto sinTheta = std::sin((y + 0.5f) / height * Pi);
      for (auto x = 0; x < width; ++x)
        data[y * width + x] = envMap->data[y * width + x].y() * sinTheta;
    }
    distrib = Distribution2D(data.get(), width, height);
  }

  void preprocess(const Scene& scene) override {
    worldDiameter = scene.getBoundingBox().pMin.length() * 2.0f;
  }

  Color3f le(const Ray& ray) const override {
    auto w = toLocal.applyV(ray.d);
    return envMap->eval(Vector2f(sphericalPhi(w) * Inv2Pi, sphericalTheta(w) * InvPi));
  }

  Color3f sample(
      const Interaction& ref,
      const Vector2f& u,
      Vector3f& wi,
      float& pdf,
      VisibilityTester& tester) const override {

    float mapPdf;
    auto uv = distrib.sampleContinuous(u, mapPdf);

    auto phi = uv.x * Pi * 2;
    auto theta = uv.y * Pi;
    auto cosTheta = std::cos(theta);
    auto sinTheta = std::sin(theta);
    if (sinTheta == 0.0f) return Color3f(0.0f);

    wi = toWorld.applyV(sphericalDirection(sinTheta, cosTheta, phi));
    pdf = mapPdf / (2 * Pi * Pi * sinTheta);
    tester = VisibilityTester(ref, ref.p + wi * worldDiameter);

    return envMap->eval(uv);
  }

  float pdf(const Vector3f& _w) const override {
    auto w = toLocal.applyV(_w);
    auto phi = sphericalPhi(w);
    auto theta = sphericalTheta(w);
    auto sinTheta = std::sin(theta);
    if (sinTheta == 0.0f) return 0.0f;
    return distrib.pdf(Vector2f(phi * Inv2Pi, theta * InvPi)) / (2 * Pi * Pi * sinTheta);
  }

  std::string toString() const override {
    return tfm::format(
      "EnvironmentLight[\n"
      "  envMap = %s\n"
      "]",
      indent(envMap->toString())
    );
  }

private:
  float worldDiameter;
  ImageTexture<Color3f>* envMap;
  Distribution2D distrib;
  Matrix4f toWorld, toLocal;
};

}
