#include <minpt/microfacets/trowbridge.h>

namespace minpt {

float TrowbridgeReitzDistribution::lambda(const Vector3f& w) const {
  auto absTanTheta = std::abs(tanTheta(w));
  if (std::isinf(absTanTheta)) return 0.0f;
  auto alpha = std::sqrt(cos2Phi(w) * alphaX * alphaX + sin2Phi(w) * alphaY * alphaY);
  auto alpha2Tan2Theta = (alpha * absTanTheta) * (alpha * absTanTheta);
  return (-1.0f + std::sqrt(1.0f + alpha2Tan2Theta)) / 2.0f;
}

float TrowbridgeReitzDistribution::d(const Vector3f& wh) const {
  auto tan2Theta = minpt::tan2Theta(wh);
  if (std::isinf(tan2Theta)) return 0.0f;
  auto cos4Theta = cos2Theta(wh) * cos2Theta(wh);
  auto e = (cos2Phi(wh) / (alphaX * alphaX) + sin2Phi(wh) / (alphaY * alphaY)) * tan2Theta;
  return 1.0f / (Pi * alphaX * alphaY * cos4Theta * (1 + e) * (1 + e));
}

Vector3f TrowbridgeReitzDistribution::sample(const Vector2f& u) const {
  auto cosTheta = 0.0f;
  auto phi = u[1] * Pi * 2;
  if (alphaX == alphaY) {
    auto tan2Theta = alphaX * alphaX * u[0] / (1.0f - u[0]);
    cosTheta = 1.0f / std::sqrt(1 + tan2Theta);
  } else {
    phi = std::atan(alphaY / alphaX * std::tan(2 * Pi * u[1] + 0.5f * Pi));
    if (u[1] > 0.5f) phi += Pi;
    auto sinPhi = std::sin(phi);
    auto cosPhi = std::cos(phi);
    auto alphaX2 = alphaX * alphaX;
    auto alphaY2 = alphaY * alphaY;
    auto alpha2 = 1.0f / (cosPhi * cosPhi / alphaX2 + sinPhi * sinPhi / alphaY2);
    auto tan2Theta = alpha2 * u[0] / (1 - u[0]);
    cosTheta = 1.0f / std::sqrt(1 + tan2Theta);
  }
  auto sinTheta = std::sqrt(std::max(0.0f, 1.0f - cosTheta * cosTheta));
  return sphericalDirection(sinTheta, cosTheta, phi);
}

}
