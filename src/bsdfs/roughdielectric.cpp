#include <minpt/bsdfs/roughdielectric.h>

namespace minpt {

RoughDielectric::RoughDielectric(const PropertyList& props)
    : remapRoughness(props.getBoolean("remaproughness", true))
    , kr(props.getColor3f("kr", Color3f(1.0f)))
    , kt(props.getColor3f("kt", Color3f(1.0f)))
    , intIOR(props.getFloat("intIOR", 1.5046f))
    , extIOR(props.getFloat("extIOR", 1.000277f))
    , uRoughness(std::max(0.0001f, props.getFloat("uRoughness", 0.0001f)))
    , vRoughness(std::max(0.0001f, props.getFloat("vRoughness", 0.0001f))) {

  eta = intIOR / extIOR;
  distrib.alphaX = remapRoughness ? TrowbridgeReitzDistribution::roughnessToAlpha(uRoughness) : uRoughness;
  distrib.alphaY = remapRoughness ? TrowbridgeReitzDistribution::roughnessToAlpha(vRoughness) : vRoughness;
}

float RoughDielectric::pdf(const BSDFQueryRecord& bRec) const {
  if (sameHemisphere(bRec.wo, bRec.wi)) {
    auto wh = normalize(bRec.wo + bRec.wi);
    if (wh.z < 0) wh = -wh;
    auto cosThetaH = dot(bRec.wo, wh);
    auto f = fr(cosThetaH, eta);
    return f * distrib.pdf(wh) / (4.0f * std::abs(cosThetaH));
  }

  auto eta = cosTheta(bRec.wo) > 0 ? this->eta : 1.0f / this->eta;
  auto wh = normalize(bRec.wo + bRec.wi * eta);
  if (wh.z < 0) wh = -wh;
  auto cosThetaH = dot(bRec.wo, wh);
  auto f = fr(cosThetaH, eta);
  auto cosThetaT = dot(bRec.wi, wh);
  auto sqrtDenom = cosThetaT + cosThetaH / eta;
  return (1.0f - f) * distrib.pdf(wh) * std::abs(cosThetaT) / (sqrtDenom * sqrtDenom);
}

Color3f RoughDielectric::f(const BSDFQueryRecord& bRec) const {
  if (cosTheta(bRec.wo) == 0 || cosTheta(bRec.wi) == 0)
    return Color3f(0.0f);

  if (sameHemisphere(bRec.wo, bRec.wi)) {
    auto wh = normalize(bRec.wo + bRec.wi);
    if (wh.z < 0) wh = -wh;
    auto d = distrib.d(wh);
    auto g = distrib.g(bRec.wo, bRec.wi);
    auto f = fr(dot(bRec.wo, wh), eta);
    return kr * d * g * f / (4 * cosTheta(bRec.wo) * cosTheta(bRec.wi));
  }

  auto eta = cosTheta(bRec.wo) > 0 ? this->eta : 1.0f / this->eta;
  auto wh = normalize(bRec.wo + bRec.wi * eta);
  if (wh.z < 0) wh = -wh;
  auto cosThetaH = dot(bRec.wo, wh);
  auto f = fr(cosThetaH, eta);
  if (f == 1.0f)
    return Color3f(0.0f);
  auto cosThetaT = dot(bRec.wi, wh);
  auto sqrtDenom = cosThetaT + cosThetaH / eta;
  return kt * ((1.0f - f) * distrib.d(wh) * distrib.g(bRec.wo, bRec.wi) *
      std::abs(cosThetaH * cosThetaT / (cosTheta(bRec.wo) * cosTheta(bRec.wi) * sqrtDenom * sqrtDenom)));
}

Color3f RoughDielectric::sample(BSDFQueryRecord& bRec, const Vector2f& u, float& pdf) const {
  auto wh = distrib.sample(u);
  auto cosThetaH = dot(bRec.wo, wh);
  auto f = fr(cosThetaH, eta);

  if (bRec.sampler->get1D() < f) {
    bRec.wi = reflect(bRec.wo, wh);
    if (!sameHemisphere(bRec.wo, bRec.wi))
      return Color3f(0.0f);
    pdf = f * distrib.pdf(wh) / (4.0f * std::abs(cosThetaH));
    return kr * distrib.g(bRec.wo, bRec.wi) * std::abs(cosThetaH) / (std::abs(cosTheta(bRec.wo) * cosTheta(wh)));
  }

  float cosThetaT;
  bRec.wi = refract(bRec.wo, wh, eta, cosThetaT);
  if (sameHemisphere(bRec.wo, bRec.wi))
    return Color3f(0.0f);
  auto sqrtDenom = cosThetaT + cosThetaH / eta;
  pdf = (1.0f - f) * distrib.pdf(wh) * std::abs(cosThetaT) / (sqrtDenom * sqrtDenom);
  return kt * distrib.g(bRec.wo, bRec.wi) * absdot(bRec.wo, wh) / (std::abs(cosTheta(bRec.wo) * cosTheta(wh)));
}

std::string RoughDielectric::toString() const {
  return tfm::format(
    "RoughDielectric[\n"
    "  kr = %s,\n"
    "  kt = %s,\n"
    "  intIOR = %f,\n"
    "  extIOR = %f,\n"
    "  uRoughness = %f,\n"
    "  vRoughness = %f,\n"
    "]",
    kr.toString(), kt.toString(),
    intIOR, extIOR,
    uRoughness, vRoughness
  );
}

}
