#pragma once

#include <minpt/math/color.h>
#include <minpt/math/vector.h>

namespace minpt {

class Bitmap : public Eigen::Array<Color3f, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> {
public:
  using Base = Eigen::Array<Color3f, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;
  using Base::Base;

  /**
   * \brief Allocate a new bitmap of the specified size
   *
   * The contents will initially be undefined, so make sure
   * to call \ref clear() if nessary
   */
  Bitmap(const Vector2i& size = Vector2i(0, 0)) : Base(size.y(), size.x())
  { }

  /// Load an OpenEXR file
  Bitmap(const std::string& filename);

  /// Save the bitmap as an EXR file
  void save(const std::string& filename);

  static float inverseGammaCorrect(float v) {
    if (v <= 0.04045f) return v / 12.92f;
    return std::pow((v + 0.055f) / 1.055f, 2.4f);
  }

  static Color3f inverseGammaCorrect(const Color3f& s) {
    return {
      inverseGammaCorrect(s[0]),
      inverseGammaCorrect(s[1]),
      inverseGammaCorrect(s[2])
    };
  }

  static float gammaCorrect(float v) {
    if (v <= 0.0031308f) return 12.92f * v;
    return 1.055f * std::pow(v, (1.f / 2.4f)) - 0.055f;
  }

  static Color3f gammaCorrect(const Color3f& color) {
    return Color3f(
      gammaCorrect(color[0]),
      gammaCorrect(color[1]),
      gammaCorrect(color[2])
    );
  }

private:
  void readImagePNG(const std::string& filename);
  void readImageEXR(const std::string& filename);
  void writeImagePNG(const std::string& filename) const;
  void writeImageEXR(const std::string& filename) const;
};

}
