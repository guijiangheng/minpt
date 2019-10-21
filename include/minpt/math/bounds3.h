#pragma once

#include <minpt/core/ray.h>

namespace minpt {

template <typename T>
class Bounds3 {
public:
  Bounds3() noexcept
    : pMin(std::numeric_limits<T>::max())
    , pMax(std::numeric_limits<T>::lowest())
  { }

  explicit Bounds3(const Vector3<T>& p) noexcept : pMin(p), pMax(p)
  { }

  Bounds3(const Vector3<T>& pMin, const Vector3<T>& pMax) noexcept : pMin(pMin), pMax(pMax)
  { }

  void reset() {
    pMin = Vector3<T>(std::numeric_limits<T>::max());
    pMax = Vector3<T>(std::numeric_limits<T>::lowest());
  }

  const Vector3<T>& operator[](int index) const {
    return (&pMin)[index];
  }

  Vector3<T>& operator[](int index) {
    return (&pMin)[index];
  }

  Bounds3& merge(const Vector3<T>& p) {
    pMin = min(pMin, p);
    pMax = max(pMax, p);
    return *this;
  }

  Bounds3& merge(const Bounds3& b) {
    pMin = min(pMin, b.pMin);
    pMax = max(pMax, b.pMax);
    return *this;
  }

  Vector3<T> centroid() const {
    return (pMin + pMax) / 2;
  }

  Vector3<T> diag() const {
    return pMax - pMin;
  }

  Vector3<T> offset(const Vector3<T>& p) const {
    return (p - pMin) / diag();
  }

  T area() const {
    auto d = diag();
    return 2 * (d.x * d.y + d.x * d.z + d.y * d.z);
  }

  int majorAxis() const {
    auto d = diag();
    if (d.x > d.y) return d.x > d.z ? 0 : 2;
    return d.y > d.z ? 1 : 2;
  }

  bool intersect(const Ray& ray, const Vector3f& invDir, const int dirIsNeg[3]) const {
    auto& b = *this;
    auto tMin = (b[dirIsNeg[0]].x - ray.o.x) * invDir.x;
    auto tMax = (b[1 - dirIsNeg[0]].x - ray.o.x) * invDir.x;
    if (tMax <= 0 || tMin >= ray.tMax) return false;
    tMin = std::max(tMin, 0.0f);
    tMax = std::min(tMax, ray.tMax);
    auto tyMin = (b[dirIsNeg[1]].y - ray.o.y) * invDir.y;
    auto tyMax = (b[1 - dirIsNeg[1]].y - ray.o.y) * invDir.y;
    if (tyMax <= tMin || tyMin >= tMax) return false;
    tMin = std::max(tMin, tyMin);
    tMax = std::min(tMax, tyMax);
    auto tzMin = (b[dirIsNeg[2]].z - ray.o.z) * invDir.z;
    auto tzMax = (b[1 - dirIsNeg[2]].z - ray.o.z) * invDir.z;
    if (tzMin >= tMax || tzMax <= tMin) return false;
    return true;
  }

  bool operator==(const Bounds3& b) const {
    return pMin == b.pMin && pMax == b.pMax;
  }

  bool operator!=(const Bounds3& b) const {
    return pMin != b.pMin || pMax != b.pMax;
  }

  std::string toString() const {
    return tfm::format(
      "Bounds[pMin=%s, pMax=%s]",
      pMin.toString(),
      pMax.toString()
    );
  }

public:
  Vector3<T> pMin, pMax;
};

using Bounds3i = Bounds3<int>;
using Bounds3f = Bounds3<float>;

template <typename T>
Bounds3<T> merge(const Bounds3<T>& b, const Vector3<T>& p) {
  return Bounds3<T>(min(b.pMin, p), max(b.pMax, p));
}

template <typename T>
Bounds3<T> merge(const Bounds3<T>& a, const Bounds3<T>& b) {
  return Bounds3<T>(min(a.pMin, b.pMin), max(a.pMax, b.pMax));
}

}
