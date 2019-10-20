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
    : camera(nullptr)
    , sampler(nullptr)
    , integrator(nullptr)
    , accel(nullptr)
    , outputName(props.getString("outputName", ""))
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

    std::cout << std::endl;
    std::cout << "Configuration: " << toString() << std::endl;
    std::cout << std::endl;
  }

  bool intersect(const Ray3f& ray) const {
    return accel->intersect(ray);
  }

  bool intersect(const Ray3f& ray, Interaction& isect) const {
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
  Camera* camera;
  Sampler* sampler;
  Integrator* integrator;
  Accelerator* accel;
  std::string outputName;
};

}
