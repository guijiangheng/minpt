#pragma once

#include <limits>
#include <tinyformat.h>
#include <minpt/math/vector.h>

namespace minpt {

template <typename _PointType>
class BoundingBox {
public:
  enum {
    Dimension = _PointType::Dimension
  };
  using PointType = _PointType;
  using Scalar = typename PointType::Scalar;

  BoundingBox() noexcept {
    reset();
  }

  BoundingBox(const PointType& p) noexcept : min(p), max(p)
  { }

  BoundingBox(const PointType& min, const PointType& max) noexcept : min(min), max(max)
  { }

  void reset() {
    min.setConstant( std::numeric_limits<Scalar>::infinity());
    max.setConstant(-std::numeric_limits<Scalar>::infinity());
  }

  Scalar getVolume() const {
    return (max - min).prod();
  }

  Scalar getSurfaceArea() const {
    static_assert(Dimension == 3);
    auto d = max - min;
    return (Scalar)2 * (d[0] * d[1] + d[0] * d[2] + d[1] * d[2]);
  }

  PointType getCenter() const {
    return (min + max) / 2;
  }

  int getMajorAxis() const {
    auto d = max - min;
    auto largest = 0;
    for (auto i = 1; i < Dimension; ++i)
      if (d[i] > d[largest]) largest = i;
    return largest;
  }

  PointType getExtents() const {
    return max - min;
  }

  void expandBy(const PointType& p) {
    min = min.cwiseMin(p);
    max = max.cwiseMax(p);
  }

  void expandBy(const BoundingBox& b) {
    min = min.cwiseMin(b.min);
    max = max.cwiseMax(b.max);
  }

  static BoundingBox merge(const BoundingBox& b, const PointType& p) {
    return BoundingBox(b.min.cwiseMin(p), b.max.cwiseMax(p));
  }

  static BoundingBox merge(const BoundingBox& a, const BoundingBox& b) {
    return BoundingBox(
      a.min.cwiseMin(b.min),
      a.max.cwiseMax(b.max)
    );
  }

  bool isValid() const {
    return (max.array() >= min.array()).all();
  }

  bool operator==(const BoundingBox& b) const {
    return min == b.min && max == b.max;
  }

  bool operator!=(const BoundingBox& b) const {
    return min != b.min || max != b.max;
  }

  std::string toString() const {
    if (!isValid()) return "BoundingBox[invalid]";
    return tfm::format("BoundingBox[min=%s, max=%s]", min.toString(), max.toString());
  }

public:
  PointType min;
  PointType max;
};

using Bounds2i = BoundingBox<Vector2i>;
using Bounds2f = BoundingBox<Vector2f>;
using Bounds2d = BoundingBox<Vector2d>;

using Bounds3i = BoundingBox<Vector3i>;
using Bounds3f = BoundingBox<Vector3f>;
using Bounds3d = BoundingBox<Vector3d>;

}
