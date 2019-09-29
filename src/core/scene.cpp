#include <tbb/parallel_for.h>
#include <minpt/core/scene.h>
#include <minpt/utils/bitmap.h>

namespace minpt {

void Scene::render(const std::string& outputName) const {
  constexpr auto TILE_SIZE = 16;
  auto outputSize = camera->outputSize;
  Bitmap bitmap(outputSize);
  Vector2i tiles(
    (outputSize.x() + TILE_SIZE - 1) / TILE_SIZE,
    (outputSize.y() + TILE_SIZE - 1) / TILE_SIZE);
  tbb::parallel_for(tbb::blocked_range<int>{0, tiles.x() * tiles.y()}, [&](auto& range) {
    auto sampler = this->sampler->clone();
    for (auto i = range.begin(); i < range.end(); ++i) {
      Vector2i block(i % tiles.x(), i / tiles.x());
      sampler->prepare(block);
      auto x0 = TILE_SIZE * block.x();
      auto y0 = TILE_SIZE * block.y();
      auto x1 = std::min(x0 + TILE_SIZE, outputSize.x());
      auto y1 = std::min(y0 + TILE_SIZE, outputSize.y());
      for (auto y = y0; y < y1; ++y)
        for (auto x = x0; x < x1; ++x) {
          Color3f color(0.0f);
          sampler->startPixel();
          do {
            auto cameraSample = sampler->getCameraSample(Vector2i(x, y));
            auto ray = camera->generateRay(cameraSample);
            color += integrator->li(ray, *this, *sampler);
          } while (sampler->startNextSample());
          bitmap(y, x) = color / sampler->samplesPerPixel;
        }
    }
  });
  bitmap.save(outputName);
}

}
