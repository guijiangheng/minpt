#include <minpt/core/sampling.h>
#include <minpt/bsdfs/plastic.h>

namespace minpt {

Plastic::Plastic(const PropertyList& props)
    : remapRoughness(props.getBoolean("remapRoughness", true))
    , roughness(props.getFloat("roughness", 0.1f))
    , eta(props.getFloat("eta", 1.504183f))
    , kd(props.getColor3f("kd", Color3f(0.5f)))
    , ks(1.0f - kd.maxComponent()) {

  auto alpha = remapRoughness ? BeckmannDistribution::roughnessToAlpha(roughness) : roughness;
  distrib.alphaX = distrib.alphaY = alpha;
}

float Plastic::fr(float cosThetaI, float eta) {
  auto sinThetaI = std::sqrt(std::max(0.0f, 1.0f - cosThetaI * cosThetaI));
  auto sinThetaT = sinThetaI / eta;
  if (sinThetaT >= 1.0f) return 1.0f;
  auto cosThetaT = std::sqrt(std::max(0.0f, 1.0f - sinThetaT * sinThetaT));
  auto rs = (eta * cosThetaI - cosThetaT) / (eta * cosThetaI + cosThetaT);
  auto rp = (cosThetaI - eta * cosThetaT) / (cosThetaI + eta * cosThetaT);
  return (rs * rs + rp * rp) / 2.0f;
}

Color3f Plastic::f(const Vector3f& wo, const Vector3f& wi) const {
  if (!sameHemisphere(wo, wi))
    return Color3f(0.0f);
  auto wh = normalize(wo + wi);
  auto d = distrib.d(wh);
  auto g = distrib.g(wo, wi);
  auto f = fr(dot(wo, wh), eta);
  return kd * InvPi + Color3f(d * g * f * ks / (4 * cosTheta(wo) * cosTheta(wi)));
}

Color3f Plastic::sample(
    const Vector2f& u,
    const Vector3f& wo,
    Vector3f& wi,
    float& pdf,
    float& etaScale) const {

  etaScale = 1.0f;

  if (u.x < ks) {
    auto uRemapped = Vector2f(u.x / ks, u.y);
    auto wh = distrib.sample(u);
    wi = reflect(wo, wh);
    if (!sameHemisphere(wo, wi)) {
      pdf = 0.0f;
      return Color3f(0.0f);
    }
  } else {
    auto uRemapped = Vector2f((u.x - ks) / (1.0f - ks), u.y);
    wi = cosineSampleHemisphere(uRemapped);
    if (wo.z < 0.0f) wi.z = -wi.z;
  }

  pdf = this->pdf(wo, wi);

  return f(wo, wi) / pdf * absCosTheta(wi);
}

std::string Plastic::toString() const {
  return tfm::format(
    "Plastic[\n"
    "  roughness = %f,\n"
    "  remapRoughness = %s,\n"
    "  eta = %f,\n"
    "  kd = %s,\n"
    "  microfacet = %s\n"
    "]",
    roughness,
    remapRoughness ? "true" : "false",
    eta,
    kd.toString(),
    distrib.toString()
  );
}

}
