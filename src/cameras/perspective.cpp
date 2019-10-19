#include <minpt/math/bounds.h>
#include <minpt/utils/utils.h>
#include <minpt/core/camera.h>

namespace minpt {

class PerspectiveCamera : public Camera {
public:
  PerspectiveCamera(const PropertyList& props)
      : Camera(
        props.getTransform("toWorld"),
        Vector2i(props.getVector2i("outputSize")))
      , fov(props.getFloat("fov")) {

    auto aspectInv = (float)outputSize.y() / outputSize.x();
    screenWindow = props.getBounds2f("screenWindow", Bounds2f(
      Vector2f(-1.0f, -aspectInv),
      Vector2f( 1.0f,  aspectInv)));
    Vector2f diag = screenWindow.diagonal();
    Matrix4f screenToRaster = (
      Eigen::DiagonalMatrix<float, 3>(outputSize.x() / diag.x(), -outputSize.y() / diag.y(), 1.0f) *
      Eigen::Translation3f(-screenWindow.min().x(), -screenWindow.max().y(), 0.0f)).matrix();
    Matrix4f cameraToScreen = Matrix4f::perspective(fov, 0.0001f, 1000.0f);
    rasterToCamera = (screenToRaster * cameraToScreen).inverse();
  }

  Ray3f generateRay(const CameraSample& sample) const override {
    Vector3f pFilm(sample.pFilm.x(), sample.pFilm.y(), 0);
    Vector3f pCamera = rasterToCamera.applyP(pFilm);
    Ray3f ray(Vector3f(0.0f), pCamera.normalized());
    return frame * ray;
  }

  std::string toString() const override {
    return tfm::format(
      "PerspectiveCamera[\n"
      "  frame = %s,\n"
      "  outputSize = %s,\n"
      "  screenWindow = %s,\n"
      "  fov = %f\n"
      "]",
      indent(frame.toString(), 10),
      indent(outputSize.toString()),
      indent(screenWindow.toString()),
      fov
    );
  }

private:
  Matrix4f rasterToCamera;
  Bounds2f screenWindow;
  float fov;
};

MINPT_REGISTER_CLASS(PerspectiveCamera, "perspective");

}
