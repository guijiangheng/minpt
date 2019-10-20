#pragma once

#include <tinyformat.h>
#include <Eigen/Core>

namespace minpt {

class Color3f : public Eigen::Array3f {
public:
  using Base = Eigen::Array3f;
  using Base::Base;

  explicit Color3f(float value) noexcept : Base(value, value, value)
  { }

  float& r() {
    return x();
  }

  const float& r() const {
    return x();
  }

  float& g() {
    return y();
  }

  const float& g() const {
    return y();
  }

  float& b() {
    return z();
  }

  const float& b() const {
    return z();
  }

  float luminance() const {
    return coeff(0) * 0.212671f + coeff(1) * 0.715160f + coeff(2) * 0.072169f;
  }

  std::string toString() const {
    return tfm::format("[%f, %f, %f]", coeff(0), coeff(1), coeff(2));
  }
};

}
