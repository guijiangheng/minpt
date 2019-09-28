#pragma once

#include <vector>
#include <minpt/core/mesh.h>

namespace minpt {

class Accelerator {
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
    primOffset.push_back(primOffset.back() + mesh->getPrimitiveCount());
  }

  std::uint32_t getPrimitiveCount() const {
    return primOffset.back();
  }

  Mesh* findMesh(int& index) const {
    auto itr = std::lower_bound(primOffset.begin(), primOffset.end(), index + 1) - 1;
    index -= *itr;
    return meshes[itr - primOffset.begin()];
  }

  virtual void build() = 0;

  virtual Bounds3f getBoundingBox() const = 0;

  virtual bool intersect(const Ray3f& ray) const = 0;

  virtual bool intersect(const Ray3f& ray, Interaction& isect) = 0;

  virtual std::string toString() const = 0;

private:
  std::vector<Mesh*> meshes;
  std::vector<std::uint32_t> indices;
  std::vector<std::uint32_t> primOffset;
};

}
