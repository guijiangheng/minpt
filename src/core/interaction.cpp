#include <minpt/lights/area.h>

namespace minpt {

// caller must ensure light exist
Color3f Interaction::le(const Vector3f& w) const {
  return mesh->light->le(*this, w);
}

Color3f Interaction::f(const BSDFQueryRecord& bRec) const {
  return mesh->bsdf->f(bRec);
}

float Interaction::lightPdf(const Vector3f& ref) const {
  return mesh->light->pdf(ref, *this);
}

float Interaction::scatteringPdf(const BSDFQueryRecord& bRec) const {
  return mesh->bsdf->pdf(bRec);
}

Color3f Interaction::sample(BSDFQueryRecord& bRec, const Vector2f& u, float& pdf) const {
  return mesh->bsdf->sample(bRec, u, pdf);
}

bool Interaction::isLight() const {
  return mesh->light != nullptr;
}

}
