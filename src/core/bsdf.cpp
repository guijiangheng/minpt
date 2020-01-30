#include <minpt/core/bsdf.h>

namespace minpt {

float fr(float cosThetaI, float eta) {
  if (cosThetaI < 0) {
    cosThetaI = -cosThetaI;
    eta = 1.0f / eta;
  }
  auto sinThetaI = safe_sqrt(1.0f - cosThetaI * cosThetaI);
  auto sinThetaT = sinThetaI / eta;
  if (sinThetaT >= 1.0f) return 1.0f;
  auto cosThetaT = safe_sqrt(1.0f - sinThetaT * sinThetaT);
  auto rs = (eta * cosThetaI - cosThetaT) / (eta * cosThetaI + cosThetaT);
  auto rp = (cosThetaI - eta * cosThetaT) / (cosThetaI + eta * cosThetaT);
  return (rs * rs + rp * rp) / 2.0f;
}

}
