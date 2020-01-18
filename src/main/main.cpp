#include <cstdio>
#include <thread>
#include <tbb/parallel_for.h>
#include <filesystem/resolver.h>

#include <minpt/gui/screen.h>
#include <minpt/core/timer.h>
#include <minpt/core/scene.h>
#include <minpt/core/parser.h>

using namespace minpt;

static void usage(const char* msg = nullptr) {
  if (msg)
    fprintf(stderr, "minpt: %s\n\n", msg);
  fprintf(stderr, R"(usage: minpt [<options>] <filename>
Rendering options:
    --help                        Print this help text.
    --outfile <filename>          Write the final image to the given filename.
    --transform <global|local>    Specify transform globally or locally, default globally.
)");
  exit(msg ? 1 : 0);
}

static void render(const Scene& scene, const std::string& outputName) {
  auto camera = scene.camera;
  auto integrator = scene.integrator;
  auto filter = camera->filter;
  auto outputSize = camera->outputSize;

  printf("Configuration: %s\n", scene.toString().c_str());

  constexpr auto BlockSize = 32;
  BlockGenerator generator(outputSize, BlockSize);

  ImageBlock result(outputSize, filter);
  result.clear();

  nanogui::init();
  auto screen = new Screen(result);

  std::thread renderThread([&] {
    printf("Rendering ..");
    fflush(stdout);
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
    printf(" done. (took %s)\n", timer.elapsedString().c_str());
    result.toBitmap().save(outputName);
  });

  nanogui::mainloop();
  renderThread.join();
  delete screen;
  nanogui::shutdown();
}

int main(int argc, char** argv) {
  if (argc < 2) usage();

  Options options;
  options.transformType = TransformType::Global;

  for (auto i = 1; i < argc; ++i) {
    if (!strcmp(argv[i], "--outfile")) {
      if (i + 1 == argc)
        usage("missing value after --outfile argument");
      options.outfile = argv[++i];
    } else if (!strcmp(argv[i], "--transform")) {
      if (i + 1 == argc)
        usage("missing value after --transform argument");
      options.transformType = !strcmp(argv[++i], "global") ? TransformType::Global : TransformType::Local;
    } else if (!strcmp(argv[i], "--help") || !strcmp(argv[i], "-h")) {
      usage();
    } else
      options.filename = argv[i];
  }

  try {
    filesystem::path path(options.filename);
    if (path.extension() == "xml") {
      /* Add the parent directory of the scene file to the
        file resolver. That way, the XML file can reference
        resources (OBJ files, textures) using relative paths */
      getFileResolver()->prepend(path.parent_path());

      std::unique_ptr<Object> root(loadFromXML(options));
      if (root->getClassType() != Object::EScene)
        throw Exception(
          "Fatal error: root element must be scene, currently is %s!",
          Object::classTypeName(root->getClassType())
        );

      auto scene = static_cast<Scene*>(root.get());
      auto& outputName = scene->outputName;
      if (!options.outfile.empty()) outputName = options.outfile;
      if (outputName.empty()) {
        outputName = options.filename;
        auto lastDot = outputName.find_last_of(".");
        if (lastDot != std::string::npos)
          outputName.erase(lastDot, std::string::npos);
        outputName += ".exr";
      }

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
      usage(tfm::format("unknow file \"%s\", expected an extension of type .xml", options.filename).c_str());
    }
  } catch (const std::exception& e) {
    std::cerr << "Fatal error: " << e.what() << std::endl;
    return -1;
  }

  return 0;
}
