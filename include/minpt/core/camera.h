#pragma once

#include <minpt/core/film.h>
#include <minpt/math/matrix.h>

namespace minpt {

struct CameraSample {
  Vector2f pFilm;
};

class Camera {
public:
  Camera(const Matrix4f& frame, Film& film) noexcept : frame(frame), film(film)
  { }

  virtual ~Camera() = default;

  virtual Ray3f generateRay(const CameraSample& sample) const = 0;

  virtual std::string toString() const = 0;

public:
  Matrix4f frame;
  Film& film;
};

class ProjectiveCamera : public Camera {
public:
  ProjectiveCamera(
    const Matrix4f& frame,
    Film& film,
    const Matrix4f& cameraToScreen,
    const Bounds2f& screenWindow) noexcept : Camera(frame, film) {
    auto scale =
      Eigen::Array3f((float)film.resolution.x(), (float)film.resolution.y(), 1.0f) *
      Eigen::Array3f(
        1 / (screenWindow.max().x() - screenWindow.min().x()),
        1 / (screenWindow.min().y() - screenWindow.max().y()), 1.0f);
    auto screenToRaster =
      Eigen::DiagonalMatrix<float, 3>(scale.x(), scale.y(), scale.z()) *
      Eigen::Translation3f(-screenWindow.min().x(), -screenWindow.max().y(), 0.0f);
    Matrix4f cameraToRaster = Matrix4f(screenToRaster.matrix()) * cameraToScreen;
    rasterToCamera = cameraToRaster.inverse();
  }

public:
  Matrix4f rasterToCamera;
};

}
