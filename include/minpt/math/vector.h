#pragma once

#include <tinyformat.h>
#include <Eigen/Core>

namespace minpt {

template <typename _Scalar, int _Dimension>
class Vector : public Eigen::Matrix<_Scalar, _Dimension, 1> {
public:
  enum {
    Dimension = _Dimension
  };
  using Scalar = _Scalar;
  using Base = Eigen::Matrix<Scalar, Dimension, 1>;

  Vector(Scalar value = (Scalar)0) {
    Base::setConstant(value);
  }

  Vector(Scalar x, Scalar y) : Base(x, y)
  { }

  Vector(Scalar x, Scalar y, Scalar z) : Base(x, y, z)
  { }

  Vector(Scalar x, Scalar y, Scalar z, Scalar w) : Base(x, y, z, w)
  { }

  std::string toString() const {
    std::string ret;
    for (auto i = 0; i < Dimension; ++i) {
      ret += std::to_string(this->coeff(i));
      if (i < Dimension - 1) ret += ", ";
    }
    return "[" + ret + "]";
  }
};

using Vector2i = Vector<int, 2>;
using Vector3i = Vector<int, 3>;
using Vector4i = Vector<int, 4>;

using Vector2f = Vector<float, 2>;
using Vector3f = Vector<float, 3>;
using Vector4f = Vector<float, 4>;

using Vector2d = Vector<double, 2>;
using Vector3d = Vector<double, 3>;
using Vector4d = Vector<double, 4>;

}
