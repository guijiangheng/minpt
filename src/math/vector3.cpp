#include <minpt/math/vector3.h>

namespace minpt {

bool refract(const Vector3f& wi, const Vector3f& _n, float eta, Vector3f& wt) {
  auto n = _n;
  auto cosThetaI = dot(wi, n);
  if (cosThetaI < 0) {
    n = -n;
    eta = 1.0f / eta;
    cosThetaI = -cosThetaI;
  }
  auto sin2ThetaI = std::max(0.0f, 1.0f - cosThetaI * cosThetaI);
  auto sin2ThetaT = sin2ThetaI / (eta * eta);
  if (sin2ThetaT > 1.0f) return false;
  auto cosThetaT = std::sqrt(1.0f - sin2ThetaT);
  wt = -wi / eta + n * (cosThetaI / eta - cosThetaT);
  return true;
}

}
