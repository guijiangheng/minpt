#include <minpt/math/common.h>
#include <minpt/math/vector3.h>

namespace minpt {

Vector3f refract(const Vector3f& wi, const Vector3f& n, float eta, float& cosThetaT) {
  auto cosThetaI = dot(wi, n);
  auto etaInv = cosThetaI > 0 ? 1.0f / eta : eta;
  auto sin2ThetaI = 1.0f - cosThetaI * cosThetaI;
  auto sin2ThetaT = sin2ThetaI * etaInv * etaInv;
  cosThetaT = safe_sqrt(1.0f - sin2ThetaT);
  if (cosThetaI > 0)
    cosThetaT = -cosThetaT;
  return n * (cosThetaI * etaInv + cosThetaT) - wi * etaInv;
}

}
