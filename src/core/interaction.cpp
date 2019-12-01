#include <minpt/lights/area.h>

namespace minpt {

// caller must ensure light exist
Color3f Interaction::le(const Vector3f& w) const {
  return mesh->light->le(*this, w);
}

Color3f Interaction::f(const Vector3f& woLocal, const Vector3f& wiLocal) const {
  return mesh->bsdf->f(woLocal, wiLocal);
}

float Interaction::lightPdf(const Vector3f& ref) const {
  return mesh->light->pdf(ref, *this);
}

float Interaction::scatteringPdf(const Vector3f& woLocal, const Vector3f& wiLocal) const {
  return mesh->bsdf->pdf(woLocal, wiLocal);
}

Color3f Interaction::sample(const Vector2f& u, const Vector3f& woLocal, Vector3f& wiLocal, float& pdf, float& etaScale) const {
  return mesh->bsdf->sample(u, woLocal, wiLocal, pdf, etaScale);
}

bool Interaction::isLight() const {
  return mesh->light != nullptr;
}

}
