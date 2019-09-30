#include <minpt/accelerators/bvh.h>

namespace minpt {

bool BVHAccel::intersect(const Ray3f& ray) const {
  auto mesh = meshes[0];
  for (std::uint32_t i = 0; i < mesh->getPrimitiveCount(); ++i) {
    if (mesh->intersect(i, ray)) return true;
  }
  return false;
}

bool BVHAccel::intersect(const Ray3f& ray, Interaction& isect) const {
  bool hit = false;
  auto mesh = meshes[0];
  std::uint32_t triIndex = 0;
  for (std::uint32_t i = 0; i < mesh->getPrimitiveCount(); ++i) {
    if (mesh->intersect(i, ray, isect)) {
      hit = true;
      triIndex = i;
    }
  }
  if (hit)
    mesh->computeIntersection(triIndex, isect);
  return hit;
}

}
