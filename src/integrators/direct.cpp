#include <minpt/integrators/direct.h>

namespace minpt {

Color3f DirectIntegrator::li(const Ray& ray, const Scene& scene, Sampler& sampler) const {
  Interaction isect;
  if (!scene.intersect(ray, isect))
    return Color3f(0.0f);

  if (isect.isLight())
    return isect.le(-ray.d);

  Color3f l(0.0f);

  float pdf;
  auto& light = scene.sampleOneLight(sampler, pdf);

  float lightPdf;
  Vector3f wi;
  VisibilityTester tester;
  auto li = light.sample(isect, sampler.get2D(), wi, lightPdf, tester) / pdf;

  auto woLocal = isect.toLocal(isect.wo);

  if (!li.isBlack()) {
    BSDFQueryRecord bRec(woLocal, isect.toLocal(wi));
    bRec.p = isect.p;
    bRec.uv = isect.uv;
    auto f = isect.f(bRec);
    if (!f.isBlack() && tester.unoccluded(scene)) {
      auto scatteringPdf = isect.scatteringPdf(bRec);
      l += f * li * absCosTheta(bRec.wi) / lightPdf * weight(lightPdf, scatteringPdf);
    }
  }

  float scatteringPdf;
  BSDFQueryRecord bRec(woLocal);
  auto f = isect.sample(bRec, sampler.get2D(), scatteringPdf);
  if (f.isBlack()) return l;

  Interaction newIsect;
  wi = isect.toWorld(bRec.wi);
  auto newRay = isect.spawnRay(wi);
  if (!scene.intersect(newRay, newIsect)) return l;
  if (!newIsect.isLight()) return l;

  li = newIsect.le(-wi);

  if (!li.isBlack()) {
    auto lightPdf = newIsect.lightPdf(isect.p);
    l += f * li * weight(scatteringPdf, lightPdf);
  }

  return l;
}

}
