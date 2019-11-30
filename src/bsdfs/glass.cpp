#include <minpt/bsdfs/glass.h>

namespace minpt {

Color3f Glass::sample(const Vector2f& u, const Vector3f& wo, Vector3f& wi, float& pdf) const {
  auto nz = 1.0f;
  auto eta = this->eta;
  auto cosThetaI = cosTheta(wo);

  if (cosThetaI < 0.0f) {
    nz = -1.0f;
    eta = 1.0f / eta;
    cosThetaI = -cosThetaI;
  }

  auto sinThetaI = std::sqrt(std::max(0.0f, 1.0f - cosThetaI * cosThetaI));
  auto sinThetaT = sinThetaI / eta;

  float fr;
  float cosThetaT;

  if (sinThetaT >= 1.0f)
    fr = 1.0f;
  else {
    cosThetaT = std::sqrt(std::max(0.0f, 1.0f - sinThetaT * sinThetaT));
    auto rs = (eta * cosThetaI - cosThetaT) / (eta * cosThetaI + cosThetaT);
    auto rp = (cosThetaI - eta * cosThetaT) / (cosThetaI + eta * cosThetaT);
    fr = (rs * rs + rp * rp) / 2.0f;
  }

  if (u.x < fr) {
    pdf = fr;
    wi = Vector3f(-wo.x, -wo.y, wo.z);
    return kr;
  } else {
    auto etaInv = 1.0f / eta;
    pdf = 1.0f - fr;
    wi = Vector3f(-wo.x * etaInv, -wo.y * etaInv, -cosThetaT * nz);
    return kt * etaInv * etaInv;
  }
}

}
