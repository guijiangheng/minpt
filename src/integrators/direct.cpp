#include <minpt/integrators/direct.h>

namespace minpt {

Color3f DirectIntegrator::li(const Ray& ray, const Scene& scene, Sampler& sampler) const {
  Interaction isect;
  if (!scene.intersect(ray, isect))
    return Color3f(0.0f);

  if (isect.mesh->light)
    return isect.mesh->light->le(isect);

  Color3f l(0.0f);

  float pdf;
  auto& light = getOneLight(scene, sampler, pdf);

  float lightPdf;
  Vector3f wi;
  VisibilityTester tester;
  auto li = light.sample(isect, sampler.get2D(), wi, lightPdf, tester) / pdf;

  auto woLocal = isect.toLocal(isect.wo);

  if (!li.isBlack()) {
    auto wiLocal = isect.toLocal(wi);
    auto f = isect.mesh->bsdf->f(woLocal, wiLocal);
    if (!f.isBlack() && tester.unoccluded(scene)) {
      auto scatteringPdf = isect.mesh->bsdf->pdf(woLocal, wiLocal);
      l += f * li * absCosTheta(wiLocal) / lightPdf * weight(lightPdf, scatteringPdf);
    }
  }

  Vector3f wiLocal;
  float scatteringPdf;
  auto f = isect.mesh->bsdf->sample(sampler.get2D(), woLocal, wiLocal, scatteringPdf);
  if (f.isBlack()) return l;

  Interaction pLight;
  wi = isect.toLocal(wiLocal);
  auto newRay = isect.spawnRay(wi);
  if (!scene.intersect(newRay, pLight)) return l;
  if (!pLight.mesh->light) return l;

  li = pLight.le(-wi);
  if (!li.isBlack()) {
    auto lightPdf = pLight.mesh->light->pdf(isect.p, pLight);
    l += f * li * absCosTheta(wiLocal) / scatteringPdf * weight(scatteringPdf, lightPdf);
  }

  return l;
}

}
