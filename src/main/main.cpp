#include <iostream>
#include <thread>
#include <tbb/parallel_for.h>
#include <filesystem/resolver.h>

#include <minpt/gui/screen.h>
#include <minpt/core/timer.h>
#include <minpt/core/scene.h>
#include <minpt/core/parser.h>

using namespace minpt;

static void render(const Scene& scene, const std::string& outputName) {
  auto camera = scene.camera;
  auto integrator = scene.integrator;
  auto filter = camera->filter;
  auto outputSize = camera->outputSize;

  integrator->preprocess(scene);

  constexpr auto BlockSize = 32;
  BlockGenerator generator(outputSize, BlockSize);

  ImageBlock result(outputSize, filter);
  result.clear();

  nanogui::init();
  auto screen = new Screen(result);

  std::thread renderThread([&] {
    std::cout << "Rendering ..";
    std::cout.flush();
    Timer timer;

    tbb::parallel_for(tbb::blocked_range<int>(0, generator.getBlockCount()), [&, camera, integrator, filter](auto& range) {
      ImageBlock block(Vector2i(BlockSize), filter);
      auto sampler = scene.sampler->clone();
      for (auto i = range.begin(); i < range.end(); ++i) {
        generator.next(block);
        sampler->prepare(block.offset);
        block.clear();
        for (auto y = 0; y < block.size.y; ++y)
          for (auto x = 0; x < block.size.x; ++x) {
            sampler->startPixel();
            do {
              auto cameraSample = sampler->getCameraSample(Vector2i(x, y) + block.offset);
              auto ray = camera->generateRay(cameraSample);
              block.put(cameraSample.pFilm, integrator->li(ray, scene, *sampler));
            } while (sampler->startNextSample());
          }
        result.put(block);
      }
    });
    std::cout << " done. (took " << timer.elapsedString() << ")" << std::endl;
    result.toBitmap().save(outputName);
  });

  nanogui::mainloop();
  renderThread.join();
  delete screen;
  nanogui::shutdown();
}

int main(int argc, char** argv) {
  if (argc != 2) {
    std::cerr << "Syntax: " << argv[0] << " <scene.xml>" << std::endl;
    return -1;
  }

  try {
    filesystem::path path(argv[1]);
    if (path.extension() == "xml") {
      /* Add the parent directory of the scene file to the
        file resolver. That way, the XML file can reference
        resources (OBJ files, textures) using relative paths */
      getFileResolver()->prepend(path.parent_path());
      std::unique_ptr<Object> root(loadFromXML(argv[1]));
      if (root->getClassType() != Object::EScene)
        throw Exception(
          "Fatal error: root element must be scene, currently is %s!",
          Object::classTypeName(root->getClassType())
        );
      auto scene = static_cast<Scene*>(root.get());
      auto& outputName = scene->outputName;
      if (outputName.empty()) {
        outputName = argv[1];
        auto lastDot = outputName.find_last_of(".");
        if (lastDot != std::string::npos)
          outputName.erase(lastDot, std::string::npos);
        outputName += ".exr";
        scene->outputName = outputName;
      }
      std::cout << "Configuration: " << scene->toString() << std::endl;
      render(*scene, outputName);
    } else if (path.extension() == "exr") {
      Bitmap bitmap(argv[1]);
      ImageBlock block(Vector2i(bitmap.cols(), bitmap.rows()), nullptr);
      block.fromBitmap(bitmap);
      nanogui::init();
      auto screen = new Screen(block);
      nanogui::mainloop();
      delete screen;
      nanogui::shutdown();
    } else {
      std::cerr
        << "Fatal error: unknown file \"" << argv[1]
        << "\", expected an extension of type .xml!" << std::endl;
      return -1;
    }
  } catch (const std::exception& e) {
    std::cerr << "Fatal error: " << e.what() << std::endl;
    return -1;
  }

  return 0;
}
