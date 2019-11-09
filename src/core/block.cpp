#include <minpt/core/block.h>

namespace minpt {

ImageBlock::ImageBlock(const Vector2i& size, const Filter* filter)
    : offset(0, 0), size(size) {
  if (filter) {
    auto filterRadius = filter->radius;
    borderSize = (int)std::ceil(filterRadius - 0.5f);
    filters = new float[FilterResolution + 1];
    filters[FilterResolution] = 0.0f;
    for (auto i = 0; i < FilterResolution; ++i)
      filters[i] = filter->eval(filterRadius * i / FilterResolution);
    lookupFactor = FilterResolution / filterRadius;
  }
  resize(size.y + 2 * borderSize, size.x + 2 * borderSize);
}

}
