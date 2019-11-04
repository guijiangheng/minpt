#pragma once

#include <vector>
#include <iostream>

#include <minpt/core/proplist.h>
#include <minpt/core/camera.h>
#include <minpt/core/sampler.h>
#include <minpt/core/integrator.h>
#include <minpt/core/accelerator.h>

namespace minpt {

class Scene : public Object {
public:
  Scene(const PropertyList& props)
    : outputName(props.getString("outputName", ""))
  { }

  ~Scene() {
    delete camera;
    delete sampler;
    delete integrator;
    delete accel;
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
    accel->build();
  }

  bool intersect(const Ray& ray) const {
    return accel->intersect(ray);
  }

  bool intersect(const Ray& ray, Interaction& isect) const {
    return accel->intersect(ray, isect);
  }

  const Bounds3f& getBoundingBox() const {
    return accel->getBoundingBox();
  }

  void render(const std::string& outputName) const;

  EClassType getClassType() const override {
    return EScene;
  }

  std::string toString() const override;

public:
  std::vector<Mesh*> meshes;
  Camera* camera = nullptr;
  Sampler* sampler = nullptr;
  Integrator* integrator = nullptr;
  Accelerator* accel = nullptr;
  std::string outputName;
};

}
