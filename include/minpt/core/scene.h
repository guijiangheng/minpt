#pragma once

#include <vector>
#include <iostream>

#include <minpt/core/proplist.h>
#include <minpt/core/light.h>
#include <minpt/core/camera.h>
#include <minpt/core/sampler.h>
#include <minpt/core/integrator.h>
#include <minpt/core/accelerator.h>

namespace minpt {

class Scene : public Object {
public:
  Scene(const PropertyList& props)
    : outputName(props.getString("outputName", ""))
    , envLight(nullptr)
  { }

  ~Scene() {
    delete camera;
    delete sampler;
    delete integrator;
    delete accel;
    for (auto light : lights)
      delete light;
  }

  void addChild(Object* child) override;

  /**
   * Initializes internal data structure (kd-tree, bvh,
   * emitter sampling data structure etc)
   */
  void activate() override {
    if (!integrator)
      throw Exception("No integrator was specified!");
    if (!camera)
      throw Exception("No camera was specified!");
    if (!sampler)
      throw Exception("No sampler was specified!");
    if (!accel)
      throw Exception("No accelerator was specified!");

    for (auto light : lights) {
      if (light->isInfinite()) {
        if (envLight)
          throw Exception("Duplicate env light, only one is supported!");
        envLight = static_cast<InfiniteLight*>(light);
      }
    }

    accel->build();
    integrator->preprocess(*this);
    if (envLight) envLight->preprocess(*this);
  }

  const Bounds3f& getBoundingBox() const {
    return accel->getBoundingBox();
  }

  const Light& sampleOneLight(Sampler& sampler, float& pdf) const {
    auto nLights = lights.size();
    pdf = 1.0f / nLights;
    auto index = std::min((std::size_t)(sampler.get1D() * nLights), nLights - 1);
    return *lights[index];
  }

  bool intersect(const Ray& ray) const {
    return accel->intersect(ray);
  }

  bool intersect(const Ray& ray, Interaction& isect) const {
    return accel->intersect(ray, isect);
  }

  EClassType getClassType() const override {
    return EScene;
  }

  std::string toString() const override;

public:
  Camera* camera = nullptr;
  Sampler* sampler = nullptr;
  Integrator* integrator = nullptr;
  Accelerator* accel = nullptr;
  std::string outputName;
  std::vector<Mesh*> meshes;
  std::vector<Light*> lights;
  InfiniteLight* envLight;
};

}
