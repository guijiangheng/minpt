#pragma once

#include <minpt/core/camera.h>

namespace minpt {

class PerspectiveCamera : public ProjectiveCamera {
public:
  PerspectiveCamera(
    const Matrix4f& frame,
    Film& film,
    const Bounds2f& screenWindow,
    float fov) noexcept
      : ProjectiveCamera(
        frame,
        film,
        Matrix4f::perspective(fov, 0.01f, 1000.0f),
        screenWindow)
      , fov(fov)
  { }

  Ray3f generateRay(const CameraSample& sample) const override {
    Vector3f pFilm(sample.pFilm.x(), sample.pFilm.y(), 0);
    auto pCamera = rasterToCamera.applyP(pFilm);
    Ray3f ray(Vector3f(0), pCamera.normalized());
    return frame * ray;
  }

  std::string toString() const override {
    return tfm::format(
      "PerspectiveCamera[\n"
      "  frame = %s,\n"
      "  film = %s,\n"
      "  fov = %f\n"
      "]",
      indent(frame.toString(), 10),
      indent(film.toString()),
      fov
    );
  }

private:
  float fov;
};

}
