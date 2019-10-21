#include <iostream>
#include <filesystem/resolver.h>
#include <minpt/core/scene.h>
#include <minpt/core/parser.h>

using namespace minpt;

int main(int argc, char** argv) {
  if (argc != 2) {
    std::cerr << "Syntax: " << argv[0] << " <scene.xml>" << std::endl;
    return -1;
  }

  filesystem::path path(argv[1]);

  try {
    if (path.extension() == "xml") {
      /* Add the parent directory of the scene file to the
        file resolver. That way, the XML file can reference
        resources (OBJ files, textures) using relative paths */
      getFileResolver()->prepend(path.parent_path());
      std::unique_ptr<Object> root(loadFromXML(argv[1]));
      if (root->getClassType() == Object::EScene) {
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
        std::cout << std::endl;
        std::cout << "Configuration: " << scene->toString() << std::endl;
        std::cout << std::endl;
        scene->render(outputName);
      }
    } else {
      std::cerr
        << "Fatal error: unknown file \"" << argv[1]
        << "\", expected an extension of type .xml" << std::endl;
      return -1;
    }
  } catch (const std::exception& e) {
    std::cerr << "Fatal error: " << e.what() << std::endl;
    return -1;
  }

  return 0;
}
