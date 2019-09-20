#pragma once

#include <minpt/math/color.h>
#include <minpt/math/vector.h>

namespace minpt {

class Bitmap : public Eigen::Array<Color3f, Eigen::Dynamic, Eigen::Dynamic> {
public:
  using Base = Eigen::Array<Color3f, Eigen::Dynamic, Eigen::Dynamic>;

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
};

}
