#include <minpt/integrators/path.h>

namespace minpt {

Color3f PathIntegrator::li(const Ray& ray, const Scene& scene, Sampler& sampler) const {
  Ray r(ray);
  Color3f l(0.0f), t(1.0f);

  Interaction isect;
  auto foundIntersection = scene.intersect(r, isect);

  for (auto bounce = 0; bounce < maxDepth; ++bounce) {
    if (!foundIntersection)
      return l;

    if (bounce == 0 && isect.isLight())
      return l + t * isect.le(-r.d);

    float pdf;
    auto& light = scene.sampleOneLight(sampler, pdf);

    float lightPdf;
    Vector3f wi;
    VisibilityTester tester;
    auto li = light.sample(isect, sampler.get2D(), wi, lightPdf, tester) / pdf;

    auto woLocal = isect.toLocal(isect.wo);

    if (!li.isBlack()) {
      auto wiLocal = isect.toLocal(wi);
      auto f = isect.f(woLocal, wiLocal);
      if (!f.isBlack() && tester.unoccluded(scene)) {
        auto scatteringPdf = isect.scatteringPdf(woLocal, wiLocal);
        l += t * f * li * absCosTheta(wiLocal) / lightPdf * weight(lightPdf, scatteringPdf);
      }
    }

    Vector3f wiLocal;
    float scatteringPdf;
    auto f = isect.sample(sampler.get2D(), woLocal, wiLocal, scatteringPdf);

    // update throughput
    t *= f;
    if (t.isBlack()) return l;

    // shoot next ray
    wi = isect.toWorld(wiLocal);
    r = isect.spawnRay(wi);

    Interaction newIsect;
    foundIntersection = scene.intersect(r, newIsect);

    if (foundIntersection) {
      if (!newIsect.isLight()) {
        isect = newIsect;
      } else {
        li = newIsect.le(-wi);
        if (!li.isBlack()) {
          auto lightPdf = newIsect.lightPdf(isect.p);
          l += t * li * weight(scatteringPdf, lightPdf);
        }
        return l;
      }
    }
  }

  return l;
}

}
