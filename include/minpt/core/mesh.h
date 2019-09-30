#pragma once

#include <Eigen/Core>
#include <Eigen/Geometry>
#include <minpt/math/bounds.h>
#include <minpt/core/object.h>
#include <minpt/core/interaction.h>

namespace minpt {

/**
 * \brief Triangle mesh
 *
 * This class stores a triangle mesh object and provides numerous functions
 * for querying the individual triangles. Subclasses of \c Mesh implement
 * the specifics of how to create its contents (e.g. by loading from an
 * external file)
 */
class Mesh : public Object {
public:
  virtual ~Mesh() = default;

  std::uint32_t getPrimitiveCount() const {
    return (std::uint32_t)f.cols();
  }

  std::uint32_t getVertexCount() const {
    return (std::uint32_t)v.cols();
  }

  float getSurfaceArea(std::uint32_t index) const {
    Vector3f a = v.col(f(0, index));
    Vector3f b = v.col(f(1, index));
    Vector3f c = v.col(f(2, index));
    return Vector3f((a - b).cross(a - c)).norm() / 2;
  }

  Bounds3f getBoundingBox(std::uint32_t index) const {
    auto& a = v.col(f(0, index));
    auto& b = v.col(f(1, index));
    Bounds3f box(a.cwiseMin(b), a.cwiseMax(b));
    return box.extend(v.col(f(2, index)));
  }

  Vector3f getCentroid(std::uint32_t index) const {
    return (
      v.col(f(0, index)) +
      v.col(f(1, index)) +
      v.col(f(2, index))) / 3;
  }

  bool intersect(std::uint32_t index, const Ray3f& ray) const;

  bool intersect(std::uint32_t index, const Ray3f& ray, Interaction& isect) const;

  void computeIntersection(std::uint32_t index, Interaction& isect) const;

  EClassType getClassType() const override {
    return EMesh;
  }

  std::string toString() const override {
    return tfm::format(
      "Mesh[\n"
      "  name = \"%s\",\n"
      "  vertexCount = %i,\n"
      "  triangleCount = %i\n"
      "]",
      name, v.cols(), f.cols()
    );
  }

protected:
  Mesh() = default;

public:
  std::string name;
  MatrixXf v;
  MatrixXf n;
  MatrixXf uv;
  MatrixXu f;
};

}
