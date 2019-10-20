#include <tbb/parallel_for.h>
#include <minpt/utils/bitmap.h>
#include <minpt/core/timer.h>
#include <minpt/core/scene.h>

namespace minpt {

void Scene::addChild(Object* child) {
  switch (child->getClassType()) {
    case EMesh: {
        if (!accel)
          throw Exception("Set accelerator first before add any mesh!");
        auto mesh = static_cast<Mesh*>(child);
        accel->addMesh(mesh);
        meshes.push_back(mesh);
      }
      break;
    case ESampler:
      if (sampler)
        throw Exception("There can only be one sampler per scene!");
      sampler = static_cast<Sampler*>(child);
      break;
    case ECamera:
      if (camera)
        throw Exception("There can only be one camera per scene!");
      camera = static_cast<Camera*>(child);
      break;
    case EIntegrator:
      if (integrator)
        throw Exception("There can only be one integrator per scene!");
      integrator = static_cast<Integrator*>(child);
      break;
    case EAccel:
      if (accel)
        throw Exception("There can only be one accelerator per scene!");
      accel = static_cast<Accelerator*>(child);
      break;
    default:
      throw Exception("Scene::addChild(<%s>) is not supported", classTypeName(child->getClassType()));
  }
}

void Scene::render(const std::string& outputName) const {
  constexpr auto TILE_SIZE = 16;
  auto outputSize = camera->outputSize;
  Bitmap bitmap(outputSize);
  Vector2i tiles(
    (outputSize.x() + TILE_SIZE - 1) / TILE_SIZE,
    (outputSize.y() + TILE_SIZE - 1) / TILE_SIZE);
  std::cout << "Rendering ..";
  std::cout.flush();
  Timer timer;
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
  std::cout << " done. (took " << timer.elapsedString() << ")" << std::endl;
  bitmap.save(outputName);
}

std::string Scene::toString() const {
  std::string string;
  for (std::size_t i = 0, length = meshes.size(); i < length; ++i) {
    string += std::string("  ") + indent(meshes[i]->toString());
    if (i + 1 < length)
      string += ",";
    string += "\n";
  }
  return tfm::format(
    "Scene[\n"
    "  integrator = %s,\n"
    "  accelerator = %s,\n"
    "  sampler = %s,\n"
    "  camera = %s,\n"
    "  meshes = {\n"
    "  %s },\n"
    "  outputName=\"%s\"\n"
    "]",
    indent(integrator->toString()),
    indent(accel->toString()),
    indent(sampler->toString()),
    indent(camera->toString()),
    indent(string),
    indent(outputName)
  );
}

MINPT_REGISTER_CLASS(Scene, "scene");

}
