#pragma once

#include <Eigen/Core>
#include <minpt/core/spectrum.h>

namespace minpt {

class Bitmap : public Eigen::Array<Spectrum, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> {
public:
  using Base = Eigen::Array<Spectrum, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;

  /**
   * \brief Allocate a new bitmap of the specified size
   *
   * The contents will initially be undefined, so make sure
   * to call \ref clear() if nessary
   */
  Bitmap(const Vector2i& size = Vector2i(0, 0)) : Base(size.y, size.x)
  { }

  /// Load an OpenEXR file
  Bitmap(const std::string& filename);

  /// Save the bitmap as an EXR file
  void save(const std::string& filename);

private:
  void readImagePFM(const std::string& filename);
  void readImagePNG(const std::string& filename);
  void readImageEXR(const std::string& filename);
  void writeImagePNG(const std::string& filename) const;
  void writeImageEXR(const std::string& filename) const;
};

}
