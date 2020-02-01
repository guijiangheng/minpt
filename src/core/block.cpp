#include <minpt/math/math.h>
#include <minpt/core/block.h>

namespace minpt {

ImageBlock::ImageBlock(const Vector2i& size, const Filter* filter)
    : offset(0, 0), size(size) {
  if (filter) {
    static constexpr auto FilterResolution = 16;
    filterRadius = filter->radius;
    borderSize = (int)std::ceil(filterRadius - 0.5f);
    filters = new float[FilterResolution + 1];
    filters[FilterResolution] = 0.0f;
    for (auto i = 0; i < FilterResolution; ++i)
      filters[i] = filter->eval(filterRadius * i / FilterResolution);
    lookupFactor = FilterResolution / filterRadius;
    auto weightSize = (int)std::ceil(2 * filterRadius) + 1;
    weightsX = new float[weightSize];
    weightsY = new float[weightSize];
  }
  resize(size.y + 2 * borderSize, size.x + 2 * borderSize);
}

void ImageBlock::put(const Vector2f& pos, const Spectrum& value) {
  if (!value.isValid()) {
    std::cerr << "Integrator: computed a invalid radiance value: " << value.toString() << std::endl;
    return;
  }
  Vector2f p(pos.x - 0.5f - offset.x + borderSize, pos.y - 0.5f - offset.y + borderSize);
  Bounds2i bounds(
    Vector2i( std::ceil(p.x - filterRadius),  std::ceil(p.y - filterRadius)),
    Vector2i(std::floor(p.x + filterRadius), std::floor(p.y + filterRadius))
  );
  bounds.pMin = minpt::max(bounds.pMin, Vector2i(0));
  bounds.pMax = minpt::min(bounds.pMax, Vector2i(cols() - 1, rows() - 1));
  for (auto x = bounds.pMin.x, idx = 0; x <= bounds.pMax.x; ++x)
    weightsX[idx++] = filters[(int)(std::abs(x - p.x) * lookupFactor)];
  for (auto y = bounds.pMin.y, idy = 0; y <= bounds.pMax.y; ++y)
    weightsY[idy++] = filters[(int)(std::abs(y - p.y) * lookupFactor)];
  for (auto y = bounds.pMin.y, yr = 0; y <= bounds.pMax.y; ++y, ++yr)
    for (auto x = bounds.pMin.x, xr = 0; x <= bounds.pMax.x; ++x, ++xr)
      coeffRef(y, x) += Color4f(value) * (weightsX[xr] * weightsY[yr]);
}

void ImageBlock::put(const ImageBlock& b) {
  auto size = b.size + Vector2i(2 * b.borderSize);
  tbb::mutex::scoped_lock lock(mutex);
  block(b.offset.y, b.offset.x, size.y, size.x) += b.topLeftCorner(size.y, size.x);
}

}
