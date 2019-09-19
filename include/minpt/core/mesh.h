#pragma once

#include <minpt/math/math.h>

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

protected:
  Mesh() = default;
  virtual ~Mesh() = default;

protected:
  std::string name;
  MatrixXf v;
  MatrixXf n;
  MatrixXf uv;
  MatrixXu f;
};

}
