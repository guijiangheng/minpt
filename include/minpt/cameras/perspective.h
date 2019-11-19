#pragma once

#include <minpt/math/math.h>
#include <minpt/utils/utils.h>
#include <minpt/core/camera.h>

namespace minpt {

class PerspectiveCamera : public Camera {
public:
  PerspectiveCamera(const PropertyList& props);

  Ray generateRay(const CameraSample& sample) const override {
    Vector3f pFilm(sample.pFilm.x, sample.pFilm.y, 0.0f);
    Vector3f pCamera = rasterToCamera.applyP(pFilm);
    Ray ray(Vector3f(0.0f), normalize(pCamera));
    return frame(ray);
  }

  std::string toString() const override;

private:
  Matrix4f rasterToCamera;
  Bounds2f screenWindow;
  float fov;
};

}
