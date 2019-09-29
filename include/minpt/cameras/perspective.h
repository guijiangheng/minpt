#pragma once

#include <minpt/utils/utils.h>
#include <minpt/core/camera.h>

namespace minpt {

class PerspectiveCamera : public Camera {
public:
  PerspectiveCamera(
      const Matrix4f& frame,
      const Vector2i& outputSize,
      float fov) noexcept : Camera(frame, outputSize), fov(fov) {
    auto aspect = outputSize.x() / (float)outputSize.y();
    Matrix4f screenToRaster = (
      Eigen::DiagonalMatrix<float, 3>(0.5f, -0.5f * aspect, 1.0f) *
      Eigen::Translation3f(1.0f, -1.0f / aspect, 0.0f)).matrix();
    Matrix4f cameraToScreen = Matrix4f::perspective(radians(fov / 2.0f), 0.01f, 1000.0f);
    rasterToCamera = (screenToRaster * cameraToScreen).inverse();
  }

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
      "  outputSize = %s,\n"
      "  fov = %f\n"
      "]",
      indent(frame.toString(), 10),
      indent(outputSize.toString()),
      fov
    );
  }

private:
  Matrix4f rasterToCamera;
  float fov;
};

}
