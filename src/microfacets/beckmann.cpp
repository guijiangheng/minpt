#include <minpt/math/math.h>
#include <minpt/microfacets/beckmann.h>

namespace minpt {

 float BeckmannDistribution::d(const Vector3f& wh) const {
  auto tan2Theta = minpt::tan2Theta(wh);
  if (std::isinf(tan2Theta)) return 0;
  auto cos4Theta = cos2Theta(wh) * cos2Theta(wh);
  if (alphaX == alphaY) {
    auto inv = 1 / (alphaX * alphaX);
    return std::exp(-tan2Theta * inv) * InvPi * inv / cos4Theta;
  }
  return std::exp(-tan2Theta * (cos2Theta(wh) / (alphaX * alphaX) +
                                sin2Theta(wh) / (alphaY * alphaY))) /
         (Pi * alphaX * alphaY * cos4Theta);
 }

Vector3f BeckmannDistribution::sample(const Vector2f& u) const {
  float phi, tan2Theta;
  auto logSample = std::log(u[0]);
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
  auto cosTheta = 1.0f / std::sqrt(1 + tan2Theta);
  auto sinTheta = std::sqrt(std::max(0.0f, 1.0f - cosTheta * cosTheta));
  return sphericalDirection(sinTheta, cosTheta, phi);
}

}
