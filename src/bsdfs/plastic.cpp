#include <minpt/core/sampling.h>
#include <minpt/bsdfs/plastic.h>

namespace minpt {

Plastic::Plastic(const PropertyList& props)
    : remapRoughness(props.getBoolean("remapRoughness", true))
    , roughness(props.getFloat("roughness", 0.1f))
    , eta(props.getFloat("eta", 1.504183f))
    , kd(props.getColor3f("kd", Color3f(0.5f)))
    , ks(props.getFloat("ks", 1.0f - kd.maxComponent())) {

  auto alpha = remapRoughness ? TrowbridgeReitzDistribution::roughnessToAlpha(roughness) : roughness;
  distrib.alphaX = distrib.alphaY = alpha;
}

Color3f Plastic::f(const BSDFQueryRecord& bRec) const {
  if (!sameHemisphere(bRec.wo, bRec.wi))
    return Color3f(0.0f);
  auto wh = normalize(bRec.wo + bRec.wi);
  auto d = distrib.d(wh);
  auto g = distrib.g(bRec.wo, bRec.wi);
  auto f = fr(dot(bRec.wo, wh), eta);
  return kd * InvPi + Color3f(d * g * f * ks / (4 * cosTheta(bRec.wo) * cosTheta(bRec.wi)));
}

Color3f Plastic::sample(BSDFQueryRecord& bRec, const Vector2f& u, float& pdf) const {
  if (u.x < ks) {
    auto uRemapped = Vector2f(u.x / ks, u.y);
    auto wh = distrib.sample(uRemapped);
    bRec.wi = reflect(bRec.wo, wh);
    if (!sameHemisphere(bRec.wo, bRec.wi))
      return Color3f(0.0f);
  } else {
    auto uRemapped = Vector2f((u.x - ks) / (1.0f - ks), u.y);
    bRec.wi = cosineSampleHemisphere(uRemapped);
    if (bRec.wo.z < 0.0f) bRec.wi.z = -bRec.wi.z;
  }

  pdf = this->pdf(bRec);

  return f(bRec) / pdf * absCosTheta(bRec.wi);
}

std::string Plastic::toString() const {
  return tfm::format(
    "Plastic[\n"
    "  roughness = %f,\n"
    "  remapRoughness = %s,\n"
    "  eta = %f,\n"
    "  kd = %s,\n"
    "  ks = %f,\n"
    "  microfacet = %s\n"
    "]",
    roughness,
    remapRoughness ? "true" : "false",
    eta,
    kd.toString(),
    ks,
    distrib.toString()
  );
}

}
