#pragma once

#include <Eigen/Core>
#include <tinyformat.h>
#include <minpt/math/math.h>
#include <minpt/math/bounds.h>

namespace minpt {

/**
 * \brief Triangle mesh
 *
 * This class stores a triangle mesh object and provides numerous functions
 * for querying the individual triangles. Subclasses of \c Mesh implement
 * the specifics of how to create its contents (e.g. by loading from an
 * external file)
 */
class Mesh {
public:
  uint32_t getPrimitiveCount() const {
    return (uint32_t)f.cols();
  }

  uint32_t getVertexCount() const {
    return (uint32_t)v.cols();
  }

  float getSurfaceArea(uint32_t index) const {
    Vector3f a = v.col(f(0, index));
    Vector3f b = v.col(f(1, index));
    Vector3f c = v.col(f(2, index));
    return Vector3f((a - b).cross(a - c)).norm() / 2;
  }

  Bounds3f getBounds(uint32_t index) const {
    Bounds3f b(v.col(f(0, index)));
    b.expandBy(v.col(f(1, index)));
    return Bounds3f::merge(b, v.col(f(2, index)));
  }

  Vector3f getCentroid(uint32_t index) const {
    return (
      v.col(f(0, index)) +
      v.col(f(1, index)) +
      v.col(f(2, index))) / 3;
  }

  bool intersect(uint32_t index, const Ray3f& ray) const;

  std::string toString() const {
    return tfm::format(
      "Mesh[\n"
      "  name = \"%s\",\n"
      "  vertexCount = %i,\n"
      "  triangleCount = %i,\n"
      "]",
      name, v.cols(), f.cols()
    );
  }

protected:
  Mesh() = default;
  virtual ~Mesh() = default;

public:
  std::string name;
  MatrixXf v;
  MatrixXf n;
  MatrixXf uv;
  MatrixXu f;
};

}
