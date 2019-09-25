#pragma once

#include <minpt/math/vector.h>

namespace minpt {

template <typename _VectorType>
class Ray {
public:
  using VectorType = _VectorType;
  using Scalar = typename VectorType::Scalar;

  Ray(const VectorType& o,
      const VectorType& d,
      Scalar tMax = std::numeric_limits<Scalar>::infinity()) noexcept
    : o(o), d(d), tMax(tMax)
  { }

  VectorType operator()(Scalar t) const {
    return o + t * d;
  }

  std::string toString() const {
    return tfm::format(
      "Ray[\n"
      "  o = %s,\n"
      "  d = %s,\n"
      "  tMax = %f\n"
      "]", o.toString(), d.toString(), tMax
    );
  }

public:
  VectorType o;
  VectorType d;
  mutable Scalar tMax;
};

using Ray2f = Ray<Vector2f>;
using Ray2d = Ray<Vector2d>;

using Ray3f = Ray<Vector3f>;
using Ray3d = Ray<Vector3d>;

}
