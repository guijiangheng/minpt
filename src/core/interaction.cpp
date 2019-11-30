#include <minpt/lights/area.h>

namespace minpt {

Color3f Interaction::le(const Vector3f& w) const {
  if (mesh->light)
    return mesh->light->le(*this, w);
  return Color3f(0.0f);
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

Color3f Interaction::sample(const Vector2f& u, const Vector3f& woLocal, Vector3f& wiLocal, float& scatteringPdf) const {
  return mesh->bsdf->sample(u, woLocal, wiLocal, scatteringPdf);
}

bool Interaction::isLight() const {
  return mesh->light != nullptr;
}

}
