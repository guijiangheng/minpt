#include <minpt/microfacets/trowbridge.h>

namespace minpt {

float TrowbridgeReitzDistribution::smithG1(const Vector3f& w, const Vector3f& wh) const {
  if (dot(w, wh) * cosTheta(w) <= 0)
    return 0.0f;

  // Perpendicular incidence -- no shadowing/masking
  auto tanTheta = std::abs(minpt::tanTheta(w));
  if (tanTheta == 0.0f)
    return 1.0f;

  auto alpha = projectRoughness(w);
  auto root = alpha * tanTheta;
  return 2.0f / (1.0f + hypot2(1.0f, root));
}

float TrowbridgeReitzDistribution::d(const Vector3f& wh) const {
  if (cosTheta(wh) <= 0)
    return 0.0f;

  auto cosTheta2 = cos2Theta(wh);
  auto exponent = ((wh.x * wh.x) / (alphaU * alphaU) +
                   (wh.y * wh.y) / (alphaV * alphaV)) / cosTheta2;
  auto root = (1.0f + exponent) * cosTheta2;
  auto ret = 1.0f / (Pi * alphaU * alphaV * root * root);

  // Prevent potential numerical issues in other stages of the model
  if (ret * cosTheta(wh) < 1e-20f)
    return 0.0f;

  return ret;
}

Vector3f TrowbridgeReitzDistribution::sample(const Vector2f& u, float* pdf) const {
  float alpha2, sinPhi, cosPhi;

  if (isIsotropic()) {
    sincos(u[1] * Pi * 2, sinPhi, cosPhi);
    alpha2 = alphaU * alphaU;
  } else {
    auto phi = std::atan(alphaV / alphaU * std::tan(2 * Pi * u[1] + 0.5f));
    if (u[1] > 0.5f) phi += Pi;
    sincos(phi, sinPhi, cosPhi);
    auto u = cosPhi / alphaU;
    auto v = sinPhi / alphaV;
    alpha2 = 1.0f / (u * u + v * v);
  }

  auto tanTheta2 = alpha2 * u[0] / (1.0f - u[0]);
  auto cosTheta = 1.0f / std::sqrt(1.0f + tanTheta2);
  auto sinTheta = safe_sqrt(1.0f - cosTheta * cosTheta);

  if (pdf) {
    auto temp = 1 + tanTheta2 / alpha2;
    *pdf = InvPi / (alphaU * alphaV * cosTheta * cosTheta * cosTheta * temp * temp);
    if (*pdf < 1e-20f)
      *pdf = 0.0f;
  }

  return Vector3f(sinTheta * cosPhi, sinTheta * sinPhi, cosTheta);
}

}
