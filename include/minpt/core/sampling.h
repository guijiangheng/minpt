#include <minpt/math/math.h>
#include <minpt/math/vector3.h>

namespace minpt {

inline Vector2f uniformSampleDisk(const Vector2f& u) {
  auto r = std::sqrt(u[0]);
  auto theta = 2 * Pi * u[1];
  return Vector2f(r * std::cos(theta), r * std::sin(theta));
}

inline Vector3f cosineSampleHemisphere(const Vector2f& u) {
  auto p = uniformSampleDisk(u);
  return Vector3f(p.x, p.y, std::sqrt(std::max(0.0f, 1 - p.length())));
}

}
