#pragma once

#include <Eigen/Geometry>
#include <minpt/core/ray.h>

namespace minpt {

template <typename Scalar, int Dimension>
class BoundingBox : public Eigen::AlignedBox<Scalar, Dimension> {
public:
  using Base = Eigen::AlignedBox<Scalar, Dimension>;
  using VectorType = typename Base::VectorType;

  using Base::Base;
  using Base::m_min;
  using Base::m_max;

  // allow parent class type object implcit convert to subclass
  BoundingBox(const Base& b) noexcept : Base(b)
  { }

  void reset() {
    min.setConstant( std::numeric_limits<Scalar>::infinity());
    max.setConstant(-std::numeric_limits<Scalar>::infinity());
  }

  int getMajorAxis() const {
    auto largest = 0;
    auto d = Base::diagonal();
    for (auto i = 1; i < Dimension; ++i)
      if (d[i] > d[largest]) largest = i;
    return largest;
  }

  Scalar surfaceArea() const {
    static_assert(Dimension == 3);
    auto d = Base::diagonal();
    return (d[0] * d[1] + d[0] * d[2] + d[1] * d[2]) * 2;
  }

  bool intersect(const Ray3f& ray, const Vector3f& invDir, const int dirIsNeg[3]) const {
    auto& b = *this;
    auto tMin = (b[dirIsNeg[0]].x() - ray.o.x()) * invDir.x();
    auto tMax = (b[1 - dirIsNeg[0]].x() - ray.o.x()) * invDir.x();
    if (tMax <= 0 || tMin >= ray.tMax) return false;
    tMin = std::max(tMin, 0.0f);
    tMax = std::min(tMax, ray.tMax);
    auto tyMin = (b[dirIsNeg[1]].y() - ray.o.y()) * invDir.y();
    auto tyMax = (b[1 - dirIsNeg[1]].y() - ray.o.y()) * invDir.y();
    if (tyMax <= tMin || tyMin >= tMax) return false;
    tMin = std::max(tMin, tyMin);
    tMax = std::min(tMax, tyMax);
    auto tzMin = (b[dirIsNeg[2]].z() - ray.o.z()) * invDir.z();
    auto tzMax = (b[1 - dirIsNeg[2]].z() - ray.o.z()) * invDir.z();
    if (tzMin >= tMax || tzMax <= tMin) return false;
    return true;
  }

  VectorType center() const {
    return (m_min + m_max) / 2;
  }

  VectorType& operator[](int index) {
    return (&m_min)[index];
  }

  const VectorType& operator[](int index) const {
    return (&m_min)[index];
  }

  std::string toString() const {
    return tfm::format(
      "BoundingBox[min=%s, max=%s]",
      Vector<Scalar, Dimension>(m_min).toString(),
      Vector<Scalar, Dimension>(m_max).toString()
    );
  }
};

using Bounds2i = BoundingBox<int, 2>;
using Bounds2f = BoundingBox<float, 2>;
using Bounds2d = BoundingBox<double, 2>;

using Bounds3i = BoundingBox<int, 3>;
using Bounds3f = BoundingBox<float, 3>;
using Bounds3d = BoundingBox<double, 3>;

}
