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

Color3f RoughDielectric::f(const BSDFQueryRecord& bRec) const {
  if (sameHemisphere(bRec.wo, bRec.wi)) {
    auto wh = normalize(bRec.wo + bRec.wi);
    auto d = distrib.d(wh);
    auto g = distrib.g(bRec.wo, bRec.wi);
    auto f = fr(dot(bRec.wo, faceForward(wh, Vector3f(0, 0, 1))), eta);
    auto ret = kr * (d * g * f / (4 * cosTheta(bRec.wo) * cosTheta(bRec.wi)));
    if (!ret.isValid())
      throw Exception("fuck1");
    return ret;
  }
  auto eta = cosTheta(bRec.wo) > 0 ? this->eta : 1.0f / this->eta;
  auto wh = normalize(bRec.wo + bRec.wi * eta);
  wh = faceForward(wh, bRec.wo);
  auto cosThetaH = dot(bRec.wo, wh);
  auto cosThetaT = dot(bRec.wi, wh);
  if (cosThetaH * cosThetaT > 0) return Color3f(0.0f);
  auto f = fr(cosThetaH, eta);
  if (f == 1.0f) return Color3f(0.0f);
  auto sqrtDenom = cosThetaT + cosThetaH / eta;
  auto ret = kt * ((1.0f - f) * distrib.d(wh) * distrib.g(bRec.wo, bRec.wi) * cosThetaH * cosThetaT
      / (cosTheta(bRec.wo) * cosTheta(bRec.wi) * sqrtDenom * sqrtDenom));
  if (!ret.isValid())
    throw Exception("fuck2");
  return ret;
}

Color3f RoughDielectric::sample(BSDFQueryRecord& bRec, const Vector2f& u, float& pdf) const {
  if (u.x < 0.5f) {
    auto uRemapped = Vector2f(u.x * 2.0f, u.y);
    auto wh = distrib.sample(uRemapped);
    bRec.wi = reflect(bRec.wo, wh);
    if (!sameHemisphere(bRec.wo, bRec.wi))
      return Color3f(0.0f);
    auto f = fr(dot(bRec.wo, wh), eta);
    auto ret = kr * (f * distrib.g(bRec.wo, bRec.wi) * dot(bRec.wo, wh) / cosTheta(bRec.wo) * 2.0f);
    if (!ret.isValid())
        throw Exception("fuck4");
    return  ret;
  } else {
    auto uRemapped = Vector2f((u.x - 0.5f) * 2.0f, u.y);
    auto wh = distrib.sample(uRemapped);
    if (!refract(bRec.wo, wh, eta, bRec.wi))
      return Color3f(0.0f);
    if (sameHemisphere(bRec.wo, bRec.wi))
      return Color3f(0.0f);
    auto cosThetaH = dot(bRec.wo, wh);
    auto f = fr(cosThetaH, eta);
    auto ret = kt * ((1.0f - f) * distrib.g(bRec.wo, bRec.wi) * std::abs(cosThetaH) / absCosTheta(bRec.wo) * 2.0f);
      if (!ret.isValid())
          throw Exception("fuck5");
      return  ret;
  }
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
