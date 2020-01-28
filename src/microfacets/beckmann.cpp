#include <minpt/math/math.h>
#include <minpt/microfacets/beckmann.h>

namespace minpt {

float BeckmannDistribution::smithG1(const Vector3f& w, const Vector3f& wh) const {
  if (dot(w, wh) * cosTheta(w) <= 0)
    return 0.0f;

  // Perpendicular incidence -- no shadowing/masking
  auto tanTheta = std::abs(minpt::tanTheta(w));
  if (tanTheta == 0.0f)
    return 1.0f;

  auto alpha = projectRoughness(w);
  auto a = 1.0f / (alpha * tanTheta);
  if (a >= 1.6f) return 1.0f;
  auto a2 = a * a;
  return (3.535f * a + 2.181f * a2) / (1.0f + 2.276f * a + 2.577f * a2);
}

float BeckmannDistribution::d(const Vector3f& wh) const {
  if (cosTheta(wh) <= 0)
    return 0.0f;

  auto cos2Theta = minpt::cos2Theta(wh);
  auto exponent = ((wh.x * wh.x) / (alphaU * alphaU) +
                   (wh.y * wh.y) / (alphaV * alphaV)) / cos2Theta;
  auto ret = std::exp(-exponent) / (Pi * alphaU * alphaV * cos2Theta * cos2Theta);

  // Prevent potential numerical issues in other stages of the model
  if (ret * cosTheta(wh) < 1e-20f)
    return 0.0f;

  return ret;
}

Vector3f BeckmannDistribution::sample(const Vector2f& u, float* pdf) const {
  float alpha2, sinPhi, cosPhi;

  if (isIsotropic()) {
    alpha2 = alphaU * alphaU;
    sincos(u[1] * 2 * Pi, sinPhi, cosPhi);
  } else {
    auto phi = std::atan(alphaV / alphaU * std::tan(2 * Pi * u[1] + 0.5f));
    if (u[1] > 0.5f) phi += Pi;
    sincos(phi, sinPhi, cosPhi);
    auto u = cosPhi / alphaU;
    auto v = sinPhi / alphaV;
    alpha2 = 1.0f / (u * u + v * v);
  }

  auto tan2Theta = -alpha2 * std::log(1.0f - u[0]);
  auto cosTheta = 1.0f / std::sqrt(1.0f + tan2Theta);
  auto sinTheta = safe_sqrt(1.0f - cosTheta * cosTheta);

  if (pdf) {
    *pdf = (1.0f - u[0]) / (Pi * alphaU * alphaV * cosTheta * cosTheta * cosTheta);
  // Prevent potential numerical issues in other stages of the model
    if (*pdf < 1e-20f)
      *pdf = 0.0f;
  }

  return Vector3f(sinTheta * cosPhi, sinTheta * sinPhi, cosTheta);
}

}
