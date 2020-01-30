#include <minpt/bsdfs/glass.h>

namespace minpt {

Color3f Glass::sample(BSDFQueryRecord& bRec, const Vector2f& u, float& pdf) const {
  auto nz = 1.0f;
  auto eta = this->eta;
  auto cosThetaI = cosTheta(bRec.wo);

  if (cosThetaI < 0.0f) {
    nz = -1.0f;
    eta = 1.0f / eta;
    cosThetaI = -cosThetaI;
  }

  auto sinThetaI = safe_sqrt(1.0f - cosThetaI * cosThetaI);
  auto sinThetaT = sinThetaI / eta;

  float fr;
  float cosThetaT = 0.0f;

  if (sinThetaT >= 1.0f)
    fr = 1.0f;
  else {
    cosThetaT = safe_sqrt(1.0f - sinThetaT * sinThetaT);
    auto rs = (eta * cosThetaI - cosThetaT) / (eta * cosThetaI + cosThetaT);
    auto rp = (cosThetaI - eta * cosThetaT) / (cosThetaI + eta * cosThetaT);
    fr = (rs * rs + rp * rp) / 2.0f;
  }

  if (u.x < fr) {
    pdf = fr;
    bRec.wi = Vector3f(-bRec.wo.x, -bRec.wo.y, bRec.wo.z);
    return kr;
  } else {
    bRec.etaScale = eta * eta;
    auto etaInv = 1.0f / eta;
    pdf = 1.0f - fr;
    bRec.wi = Vector3f(-bRec.wo.x * etaInv, -bRec.wo.y * etaInv, -cosThetaT * nz);
    return kt * etaInv * etaInv;
  }
}

}
