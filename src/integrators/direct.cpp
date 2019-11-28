#include <minpt/integrators/direct.h>

namespace minpt {

Color3f DirectIntegrator::li(const Ray& ray, const Scene& scene, Sampler& sampler) const {
    Interaction isect;
    if (!scene.intersect(ray, isect))
      return Color3f(0.0f);

    if (isect.mesh->light)
      return isect.mesh->light->le(isect);

    float lightPdf;
    auto& light = getOneLight(scene, sampler, lightPdf);

    float pdf;
    Vector3f wi;
    VisibilityTester tester;
    auto li = light.sample(isect, sampler.get2D(), wi, pdf, tester);

    if (li.isBlack())
      return Color3f(0.0f);

    auto wiLocal = isect.toLocal(wi);
    auto woLocal = isect.toLocal(isect.wo);
    auto f = isect.mesh->bsdf->f(woLocal, wiLocal);

    if (f.isBlack() || tester.occluded(scene))
      return Color3f(0);

    return f * li * absCosTheta(wiLocal) / pdf / lightPdf;
}

}
