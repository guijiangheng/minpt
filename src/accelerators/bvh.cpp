#include <minpt/core/objectfactory.h>
#include <minpt/core/accelerator.h>

namespace minpt {

class BVHAccel : public Accelerator {
public:
  BVHAccel(const PropertyList& props)
  { }

  void build() override
  { }

  Bounds3f getBoundingBox() const override {
    return Bounds3f();
  }

  bool intersect(const Ray3f& ray) const override {
    auto mesh = meshes[0];
    for (std::uint32_t i = 0; i < mesh->getPrimitiveCount(); ++i) {
      if (mesh->intersect(i, ray)) return true;
    }
    return false;
  }

  bool intersect(const Ray3f& ray, Interaction& isect) const override {
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

  std::string toString() const override {
    return "BVHAccel[]";
  }
};

MINPT_REGISTER_CLASS(BVHAccel, "bvh");

}
