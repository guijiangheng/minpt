#include <minpt/integrators/path.h>

namespace minpt {

Color3f PathIntegrator::li(const Ray& ray, const Scene& scene, Sampler& sampler) const {
  Ray r(ray);
  auto etaScaleFix = 1.0f;
  Color3f l(0.0f), t(1.0f), albedo(1.0f);

  Interaction isect;
  auto foundIntersection = scene.intersect(r, isect);

  for (auto bounce = 0; bounce < maxDepth; ++bounce) {
    if (!foundIntersection) return l;

    if (bounce == 0 && isect.isLight())
      return isect.le(-r.d);

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
        if (light.isDelta())
          l += albedo * f * absCosTheta(wiLocal) / lightPdf;
        else {
          auto scatteringPdf = isect.scatteringPdf(woLocal, wiLocal);
          l += albedo * f * li * absCosTheta(wiLocal) / lightPdf * weight(lightPdf, scatteringPdf);
        }
      }
    }

    Vector3f wiLocal;
    float etaScale;
    float scatteringPdf;
    auto f = isect.sample(sampler.get2D(), woLocal, wiLocal, scatteringPdf, etaScale);

    // update throughput
    albedo *= f;
    if (albedo.isBlack()) return l;

    // shoot next ray
    wi = isect.toWorld(wiLocal);
    r = isect.spawnRay(wi);

    Interaction newIsect;
    foundIntersection = scene.intersect(r, newIsect);

    if (foundIntersection) {
      if (!newIsect.isLight()) {
        isect = newIsect;
      } else {
        if (light.isDelta()) return l;
        auto li = newIsect.le(-wi);
        if (li.isBlack()) return l;
        if (isect.mesh->bsdf->isDelta())
          return l + albedo * li;
        auto lightPdf = newIsect.lightPdf(isect.p);
        return l + albedo * li * weight(scatteringPdf, lightPdf);
      }
    }

    etaScaleFix *= etaScale;
    t = albedo * etaScaleFix;
    if (bounce > 3 && t.maxComponent() < 1.0f) {
      auto q = std::max(0.05f, 1 - t.maxComponent());
      if (sampler.get1D() < q) break;
      albedo /= 1 - q;
    }
  }

  return l;
}

}
