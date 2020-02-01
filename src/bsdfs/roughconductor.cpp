#include <minpt/utils/utils.h>
#include <minpt/bsdfs/roughconductor.h>

namespace minpt {

RoughConductor::RoughConductor(const PropertyList& props)
    : filename(props.getString("filename"))
    , remapRoughness(props.getBoolean("remaproughness", true))
    , uRoughness(std::max(0.001f, props.getFloat("uRoughness", 0.001f)))
    , vRoughness(std::max(0.001f, props.getFloat("vRoughness", 0.001f)))
    , kr(props.getRGBSpectrum("kr", Spectrum(1.0f))) {
  auto resolver = getFileResolver();
  eta.fromSpectrum(InterpolatedSpectrum(resolver->resolve(filename + ".eta.spd").str()));
  k.fromSpectrum(InterpolatedSpectrum(resolver->resolve(filename + ".k.spd").str()));
  distrib.alphaU = remapRoughness ? TrowbridgeReitzDistribution::roughnessToAlpha(uRoughness) : uRoughness;
  distrib.alphaV = remapRoughness ? TrowbridgeReitzDistribution::roughnessToAlpha(vRoughness) : vRoughness;
}

float RoughConductor::pdf(const BSDFQueryRecord& bRec) const {
  if (!sameHemisphere(bRec.wo, bRec.wi))
    return 0.0f;
  auto wh = normalize(bRec.wo + bRec.wi);
  if (wh.z < 0) wh = -wh;
  return distrib.pdf(wh) / (4.0f * absdot(wh, bRec.wo));
}

Spectrum RoughConductor::f(const BSDFQueryRecord& bRec) const {
  if (!sameHemisphere(bRec.wo, bRec.wi))
    return Spectrum(0.0f);
  auto wh = normalize(bRec.wo + bRec.wi);
  if (wh.z < 0) wh = -wh;
  auto d = distrib.d(wh);
  auto g = distrib.g(bRec.wo, bRec.wi, wh);
  auto f = frConductor(absdot(bRec.wo, wh), eta, k);
  return kr * f * d * g / (4 * cosTheta(bRec.wo) * cosTheta(bRec.wi));
}

Spectrum RoughConductor::sample(BSDFQueryRecord& bRec, const Vector2f& u, float& pdf) const {
  auto wh = distrib.sample(u, &pdf);
  bRec.wi = reflect(bRec.wo, wh);
  if (!sameHemisphere(bRec.wo, bRec.wi))
    return Spectrum(0.0f);
  auto cosThetaH = absdot(bRec.wo, wh);
  pdf /= (4.0f * cosThetaH);
  auto g = distrib.g(bRec.wo, bRec.wi, wh);
  auto f = frConductor(cosThetaH, eta, k);
  return kr * f * g * cosThetaH / (std::abs(cosTheta(bRec.wo) * cosTheta(wh)));
}

std::string RoughConductor::toString() const {
  return tfm::format(
    "RoughConductor[\n"
    "  filename = \"%s\",\n"
    "  remaproughness = %s,\n"
    "  kr = %s,\n"
    "  eta = %s,\n"
    "  k = %s,\n"
    "  uRoughness = %f,\n"
    "  vRoughness = %f\n"
    "]",
    filename,
    remapRoughness ? "true" : "false",
    kr.toString(), eta.toString(), k.toString(),
    uRoughness, vRoughness
  );
}

}
