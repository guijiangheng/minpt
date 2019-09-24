#pragma once

#include <minpt/core/film.h>
#include <minpt/utils/utils.h>

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
    auto screenToRaster =
      Eigen::DiagonalMatrix<float,3 >(
        Vector3f((float)film.resolution.x(), (float)film.resolution.y(), 1) *
        Vector3f(
          1 / (screenWindow.max().x() - screenWindow.min().x()),
          1 / (screenWindow.min().y() - screenWindow.max().y()), 1)) *
      Eigen::Translation3f(-screenWindow.min().x(), -screenWindow.max().y(), 0);
    auto cameraToRaster = screenToRaster * cameraToScreen;
    rasterToCamera = cameraToScreen.inverse();
  }

  std::string toString() const override {
    return tfm::format(
      "PerspectiveCamera[\n"
      "  rasterToCamera = %s,\n"
      "  film = %s\n"
      "]",
      indent(minpt::toString(rasterToCamera), 2),
      indent(film.toString(), 2)
    );
  }

public:
  Matrix4f rasterToCamera;
};

}
