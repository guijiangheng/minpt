#include <minpt/math/math.h>
#include <minpt/microfacets/beckmann.h>

namespace minpt {

float BeckmannDistribution::lambda(const Vector3f& w) const {
  auto absTanTheta = std::abs(tanTheta(w));
  if (std::isinf(absTanTheta)) return 0.0f;
  auto alpha = std::sqrt(cos2Phi(w) * alphaX * alphaX + sin2Phi(w) * alphaY * alphaY);
  auto a = 1.0f / (alpha * absTanTheta);
  if (a >= 1.6f) return 0.0f;
  auto a2 = a * a;
  return (1 - 1.259f * a + 0.396f * a2) / (3.535f * a + 2.181f * a2);
}

 float BeckmannDistribution::d(const Vector3f& wh) const {
  auto cos2Theta = minpt::cos2Theta(wh);
  auto sin2Theta = std::max(0.0f, 1.0f - cos2Theta);
  auto tan2Theta = sin2Theta / cos2Theta;
  if (std::isinf(tan2Theta)) return 0.0f;
  auto cos4Theta = cos2Theta * cos2Theta;
  if (alphaX == alphaY) {
    auto inv = 1 / (alphaX * alphaX);
    return std::exp(-tan2Theta * inv) * InvPi * inv / cos4Theta;
  }
  return std::exp(-tan2Theta * (cos2Theta / (alphaX * alphaX) +
                                sin2Theta / (alphaY * alphaY))) /
         (Pi * alphaX * alphaY * cos4Theta);
 }

Vector3f BeckmannDistribution::sample(const Vector2f& u) const {
  float phi, tan2Theta;
  auto logSample = std::log(1.0f - u[0]);
  if (alphaX == alphaY) {
    phi = u[1] * 2 * Pi;
    tan2Theta = -alphaX * alphaX * logSample;
  } else {
    phi = std::atan(alphaY / alphaX * std::tan(2 * Pi * u[1] + 0.5f * Pi));
    if (u[1] > 0.5f) phi += Pi;
    auto sinPhi = std::sin(phi);
    auto sin2Phi = sinPhi * sinPhi;
    auto cos2Phi = 1.0f - sin2Phi;
    tan2Theta = -logSample / (cos2Phi / (alphaX * alphaX) + sin2Phi / (alphaY * alphaY));
  }
  auto cosTheta = 1.0f / std::sqrt(1.0f + tan2Theta);
  auto sinTheta = std::sqrt(std::max(0.0f, 1.0f - cosTheta * cosTheta));
  return sphericalDirection(sinTheta, cosTheta, phi);
}

}
