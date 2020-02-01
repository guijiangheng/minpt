#include <minpt/core/sampling.h>
#include <minpt/bsdfs/plastic.h>

namespace minpt {

Plastic::Plastic(const PropertyList& props)
    : remapRoughness(props.getBoolean("remapRoughness", true))
    , roughness(props.getFloat("roughness", 0.1f))
    , eta(props.getFloat("eta", 1.504183f))
    , kd(props.getRGBSpectrum("kd", Spectrum(0.5f)))
    , ks(props.getFloat("ks", 1.0f - kd.maxComponent())) {

  auto alpha = remapRoughness ? TrowbridgeReitzDistribution::roughnessToAlpha(roughness) : roughness;
  distrib.alphaU = distrib.alphaV = alpha;
}

Spectrum Plastic::f(const BSDFQueryRecord& bRec) const {
  if (!sameHemisphere(bRec.wo, bRec.wi))
    return Spectrum(0.0f);
  auto wh = normalize(bRec.wo + bRec.wi);
  if (wh.z < 0) wh = -wh;
  auto d = distrib.d(wh);
  auto g = distrib.g(bRec.wo, bRec.wi, wh);
  auto f = fr(absdot(bRec.wo, wh), eta);
  return kd * InvPi + Spectrum(d * g * f * ks / (4 * cosTheta(bRec.wo) * cosTheta(bRec.wi)));
}

Spectrum Plastic::sample(BSDFQueryRecord& bRec, const Vector2f& u, float& pdf) const {
  if (u.x < ks) {
    auto uRemapped = Vector2f(u.x / ks, u.y);
    auto wh = distrib.sample(uRemapped);
    bRec.wi = reflect(bRec.wo, wh);
    if (!sameHemisphere(bRec.wo, bRec.wi))
      return Spectrum(0.0f);
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
