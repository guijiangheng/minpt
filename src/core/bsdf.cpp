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

Spectrum frConductor(float cosThetaI, const Spectrum& eta, const Spectrum& k) {
  float cosThetaI2 = cosThetaI * cosThetaI,
        sinThetaI2 = 1.0f - cosThetaI2,
        sinThetaI4 = sinThetaI2 * sinThetaI2;
  Spectrum temp1 = eta * eta - k * k - Spectrum(sinThetaI2),
           a2pb2 = safe_sqrt(temp1 * temp1 + k * k * eta * eta * 4),
           a     = safe_sqrt((a2pb2 + temp1) * 0.5f);
  Spectrum term1 = a2pb2 + Spectrum(cosThetaI2),
           term2 = a * (2 * cosThetaI);
  Spectrum Rs2 = (term1 - term2) / (term1 + term2);
  Spectrum term3 = a2pb2 * cosThetaI2 + Spectrum(sinThetaI4),
           term4 = term2 * sinThetaI2;
  Spectrum Rp2 = Rs2 * (term3 - term4) / (term3 + term4);
  return (Rp2 + Rs2) * 0.5f;

}

}
