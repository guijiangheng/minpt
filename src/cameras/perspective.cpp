#include <minpt/math/math.h>
#include <minpt/utils/utils.h>
#include <minpt/core/camera.h>

namespace minpt {

class PerspectiveCamera : public Camera {
public:
  PerspectiveCamera(const PropertyList& props)
      : Camera(
        props.getTransform("toWorld"),
        props.getVector2i("outputSize"))
      , fov(props.getFloat("fov")) {

    auto aspectInv = (float)outputSize.y / outputSize.x;
    screenWindow = props.getBounds2f("screenWindow", Bounds2f(
      Vector2f(-1.0f, -aspectInv),
      Vector2f( 1.0f,  aspectInv)));
    auto diag = screenWindow.diag();
    Matrix4f screenToRaster =
      Matrix4f::scale(outputSize.x / diag.x, -outputSize.y / diag.y, 1.0f) *
      Matrix4f::translate(-screenWindow.pMin.x, -screenWindow.pMax.y, 0.0f);
    Matrix4f cameraToScreen = Matrix4f::perspective(fov, 0.0001f, 1000.0f);
    rasterToCamera = (screenToRaster * cameraToScreen).inverse();
  }

  Ray generateRay(const CameraSample& sample) const override {
    Vector3f pFilm(sample.pFilm.x, sample.pFilm.y, 0.0f);
    Vector3f pCamera = rasterToCamera.applyP(pFilm);
    Ray ray(Vector3f(0.0f), normalize(pCamera));
    return frame(ray);
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
