#include <minpt/integrators/path.h>

namespace minpt {

Color3f PathIntegrator::li(const Ray& ray, const Scene& scene, Sampler& sampler) const {
  Ray r(ray);
  auto etaScaleFix = 1.0f;
  Color3f l(0.0f), t(1.0f), albedo(1.0f);

  auto isDeltaBSDF = false;
  auto isDeltaLight = false;
  auto scatteringPdf = 0.0f;

  Interaction isect;
  auto envLight = scene.envLight;

  for (auto bounce = 0; bounce < maxDepth; ++bounce) {
    auto ref = isect.p;
    if (!scene.intersect(r, isect)) {
      if (!envLight || isDeltaLight) break;
      if (isDeltaBSDF) return l + albedo * envLight->le(r);
      auto lightPdf = envLight->pdf(r.d);
      return l + albedo * envLight->le(r) * weight(scatteringPdf, lightPdf);
    }

    if (isect.isLight()) {
      if (isDeltaBSDF) return l + albedo * isect.le(-r.d);
      auto lightPdf = (bounce == 0 ? 1.0f : isect.lightPdf(ref));
      return l + albedo * isect.le(-r.d) * weight(scatteringPdf, lightPdf);
    }

    if (bounce >=3 && t.maxComponent() < 1.0f) {
      auto q = std::max(0.05f, 1.0f - t.maxComponent());
      if (sampler.get1D() < q) break;
      albedo /= 1 - q;
    }

    auto woLocal = isect.toLocal(isect.wo);
    isDeltaBSDF = isect.mesh->bsdf->isDelta();

    if (!isDeltaBSDF) {
      float pdf;
      auto& light = scene.sampleOneLight(sampler, pdf);

      float lightPdf;
      Vector3f wi;
      VisibilityTester tester;
      auto li = light.sample(isect, sampler.get2D(), wi, lightPdf, tester) / pdf;
      isDeltaLight = light.isDelta();

      if (!li.isBlack()) {
        BSDFQueryRecord bRec(woLocal, isect.toLocal(wi));
        bRec.p = isect.p;
        bRec.uv = isect.uv;
        auto f = isect.f(bRec);
        if (!f.isBlack() && tester.unoccluded(scene)) {
          if (isDeltaLight)
            l += albedo * f * absCosTheta(bRec.wi) / lightPdf;
          else {
            auto scatteringPdf = isect.scatteringPdf(bRec);
            l += albedo * f * li * absCosTheta(bRec.wi) / lightPdf * weight(lightPdf, scatteringPdf);
          }
        }
      }
    }

    BSDFQueryRecord bRec(woLocal);
    bRec.p = isect.p;
    bRec.uv = isect.uv;
    auto f = isect.sample(bRec, sampler.get2D(), scatteringPdf);

    // update throughput
    albedo *= f;
    if (albedo.isBlack()) break;

    etaScaleFix *= bRec.etaScale;
    t = albedo * etaScaleFix;

    // shoot next ray
    auto wi = isect.toWorld(bRec.wi);
    r = isect.spawnRay(wi);
  }

  return l;
}

}
