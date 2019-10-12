#pragma once

#include <vector>
#include <minpt/core/camera.h>
#include <minpt/core/sampler.h>
#include <minpt/core/integrator.h>
#include <minpt/core/accelerator.h>

namespace minpt {

class Scene : public Object {
public:
  Scene(Camera* camera, Sampler* sampler, Integrator* integrator, Accelerator* accel)
    : camera(camera)
    , sampler(sampler)
    , integrator(integrator)
    , accel(accel)
  { }

  ~Scene() {
    delete camera;
    delete sampler;
    delete integrator;
    delete accel;
  }

  bool intersect(const Ray3f& ray) const {
    return accel->intersect(ray);
  }

  bool intersect(const Ray3f& ray, Interaction& isect) const {
    return accel->intersect(ray, isect);
  }

  Bounds3f getBoundingBox() const {
    return accel->getBoundingBox();
  }

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

  void addMesh(Mesh* mesh) {
    meshes.push_back(mesh);
    accel->addMesh(mesh);
  }

  void render(const std::string& outputName) const;

  EClassType getClassType() const override {
    return EScene;
  }

  std::string toString() const override {
    std::string string;
    for (std::size_t i = 0, length = meshes.size(); i < length; ++i) {
      string += std::string("  ") + indent(meshes[i]->toString());
      if (i + 1 < length)
        string += ",";
      string += "\n";
    }
    return tfm::format(
      "Scene[\n"
      "  integrator = %s,\n"
      "  accelerator = %s,\n"
      "  sampler = %s,\n"
      "  camera = %s,\n"
      "  meshes = {\n"
      "  %s }\n"
      "]",
      indent(integrator->toString()),
      indent(accel->toString()),
      indent(sampler->toString()),
      indent(camera->toString()),
      indent(string)
    );
  }

public:
  std::vector<Mesh*> meshes;
  Camera* camera;
  Sampler* sampler;
  Integrator* integrator;
  Accelerator* accel;
};

}
