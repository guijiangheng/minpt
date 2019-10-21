#pragma once

#include <vector>
#include <minpt/core/mesh.h>
#include <minpt/core/object.h>

namespace minpt {

class Accelerator : public Object {
public:
  Accelerator() noexcept {
    primOffset.push_back(0);
  }

  virtual ~Accelerator() {
    for (auto mesh : meshes)
      delete mesh;
  };

  void addMesh(Mesh* mesh) {
    meshes.push_back(mesh);
    bounds.merge(mesh->bounds);
    primOffset.push_back(primOffset.back() + mesh->getPrimitiveCount());
  }

  const Bounds3f& getBoundingBox() const {
    return bounds;
  }

  std::uint32_t getPrimitiveCount() const {
    return primOffset.back();
  }

  Mesh* findMesh(std::uint32_t& index) const {
    auto itr = std::lower_bound(primOffset.begin(), primOffset.end(), index + 1) - 1;
    index -= *itr;
    return meshes[itr - primOffset.begin()];
  }

  virtual void build() = 0;

  virtual bool intersect(const Ray& ray) const = 0;

  virtual bool intersect(const Ray& ray, Interaction& isect) const = 0;

  EClassType getClassType() const override {
    return EAccel;
  }

protected:
  Bounds3f bounds;
  std::vector<Mesh*> meshes;
  std::vector<std::uint32_t> indices;
  std::vector<std::uint32_t> primOffset;
};

}
