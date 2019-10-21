#pragma once

#include <limits>
#include <iterator>
#include <minpt/math/vector2.h>

namespace minpt {

template <typename T>
class Bounds2 {
public:
  Bounds2() noexcept
    : pMin(std::numeric_limits<T>::max())
    , pMax(std::numeric_limits<T>::lowest())
  { }

  explicit Bounds2(const Vector2<T>& p) noexcept
    : pMin(p), pMax(p)
  { }

  Bounds2(const Vector2<T>& pMin, const Vector2<T>& pMax) noexcept
    : pMin(pMin), pMax(pMax)
  { }

  Vector2<T> diag() const {
    return pMax - pMin;
  }

  T area() const {
    auto d = diag();
    return d.x * d.y;
  }

  bool isDegenerate() const {
    return pMin.x >= pMax.x || pMin.y >= pMax.y;
  }

  std::string toString() const {
    return tfm::format(
      "Bounds[pMin=%s, pMax=%s]",
      pMin.toString(),
      pMax.toString()
    );
  }

public:
  Vector2<T> pMin, pMax;
};

using Bounds2i = Bounds2<int>;
using Bounds2f = Bounds2<float>;

class Bounds2iIterator : public std::forward_iterator_tag {
public:
  Bounds2iIterator(const Bounds2i& bounds, const Vector2i& p) noexcept
    : bounds(&bounds), p(p)
  { }

  Bounds2iIterator& operator++() {
    ++p.x;
    if (p.x == bounds->pMax.x) {
      p.x = bounds->pMin.x;
      ++p.y;
    }
    return *this;
  }

  Bounds2iIterator operator++(int) {
    auto old = *this;
    ++(*this);
    return old;
  }

  const Vector2i& operator*() const {
    return p;
  }

  bool operator==(const Bounds2iIterator& rhs) const {
    return p == rhs.p;
  }

  bool operator!=(const Bounds2iIterator& rhs) const {
    return p != rhs.p;
  }

private:
  const Bounds2i* bounds;
  Vector2i p;
};

inline Bounds2iIterator begin(const Bounds2i& bounds) {
  return Bounds2iIterator(bounds, bounds.pMin);
}

inline Bounds2iIterator end(const Bounds2i& bounds) {
  auto p = bounds.isDegenerate() ?
    bounds.pMin : Vector2i(bounds.pMin.x, bounds.pMax.y);
  return Bounds2iIterator(bounds, p);
}

}
