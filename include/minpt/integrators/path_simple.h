#pragma once

#include <minpt/core/scene.h>

namespace minpt {

class PathSimpleIntegrator : public Integrator {
public:
  PathSimpleIntegrator(const PropertyList& props) : maxDepth(props.getInteger("maxDepth", 3))
  { }

  Spectrum li(const Ray& ray, const Scene& scene, Sampler& sampler) const override {
    Ray r(ray);
    auto etaScaleFix = 1.0f;
    Spectrum l(0.0f), t(1.0f), albedo(1.0f);

    Interaction isect;
    auto envLight = scene.envLight;

    for (auto bounce = 0; bounce < maxDepth; ++bounce) {
      if (!scene.intersect(r, isect)) {
        if (!envLight) break;
        return l + albedo * envLight->le(r);
      }

      if (isect.isLight())
        return l + albedo * isect.le(-r.d);

      if (bounce >= 3 && t.maxComponent() < 1.0f) {
        auto q = std::max(0.05f, 1.0f - t.maxComponent());
        if (sampler.get1D() < q) break;
        albedo /= 1 - q;
      }

      auto woLocal = isect.toLocal(isect.wo);
      BSDFQueryRecord bRec(woLocal);
      bRec.p = isect.p;
      bRec.uv = isect.uv;
      bRec.sampler = &sampler;
      float scatteringPdf;
      auto f = isect.sample(bRec, sampler.get2D(), scatteringPdf);

      albedo *= f;
      if (albedo.isBlack()) break;
      etaScaleFix *= bRec.etaScale;
      t = albedo * etaScaleFix;

      auto wi = isect.toWorld(bRec.wi);
      r = isect.spawnRay(wi);
    }

    return l;
  }

  std::string toString() const override {
    return tfm::format("PathSimpleIntegrator[maxDepth = %d]", maxDepth);
  }

public:
  int maxDepth;
};

}
