#include <fstream>
#include <pugixml.hpp>
#include <Eigen/Geometry>
#include <minpt/core/parser.h>
#include <minpt/core/objectfactory.h>

namespace minpt {

enum ETag {
  // Object class
  EScene = Object::EScene,
  EAccel,
  EMesh,
  ECamera,
  EIntegrator,
  ESampler,

  // Properties
  EBoolean = Object::EClassTypeCount,
  EInteger,
  EFloat,
  EString,
  EVector,
  EColor,
  ETransform,
  EMatrix,
  EScale,
  ERotate,
  ETranslate,
  ELookAt,

  EInvalid
};

static std::map<std::string, ETag> tags = {
  { "scene",      EScene },
  { "accel",      EAccel },
  { "mesh",       EMesh },
  { "camera",     ECamera },
  { "integrator", EIntegrator },
  { "sampler",    ESampler },
  { "boolean",    EBoolean },
  { "integer",    EInteger },
  { "float",      EFloat },
  { "string",     EString },
  { "vector",     EVector },
  { "color",      EColor },
  { "transform",  ETransform },
  { "matrix",     EMatrix },
  { "scale",      EScale },
  { "rotate",     ERotate },
  { "translate",  ETranslate },
  { "lookat",     ELookAt }
};

Object* loadFromXML(const std::string& filename) {
  pugi::xml_document doc;
  pugi::xml_parse_result result = doc.load_file(filename.c_str());

  auto offset = [&](ptrdiff_t pos) -> std::string {
    std::fstream is(filename);
    char buffer[1024];
    int line = 0, linestart = 0, offset = 0;
    while (is.good()) {
      is.read(buffer, sizeof(buffer));
      for (int i = 0; i < is.gcount(); ++i) {
        if (buffer[i] == '\n') {
          if (offset + i >= pos)
            return tfm::format("line %i, col %i", line + 1, pos - linestart);
          ++line;
          linestart = offset + i;
        }
      }
      offset += (int) is.gcount();
    }
    return "byte offset " + std::to_string(pos);
  };

  if (!result)
    throw Exception("Error while parsing \"%s\": %s (at %s)", filename, result.description(), offset(result.offset));

  Eigen::Affine3f transform;

  std::function<Object*(pugi::xml_node&, PropertyList&, ETag)> parseTag = [&](
    pugi::xml_node& node,
    PropertyList& parentProps,
    ETag parentTag) -> Object* {

    // Skip over comments
    if (node.type() == pugi::node_comment || node.type() == pugi::node_declaration)
      return nullptr;

    if (node.type() != pugi::node_element)
      throw Exception(
        "Error while parsing \"%s\": unexpected content at %s",
        filename, offset(node.offset_debug())
      );

    auto it = tags.find(node.name());
    if (it == tags.end())
      throw Exception(
        "Error while parsing \"%s\": unexpected tag \"%s\" at %s",
        filename, node.name(), offset(node.offset_debug())
      );

    auto tag = it->second;

    auto hasParent            = parentTag != EInvalid;
    auto parentIsObject       = parentTag < Object::EClassTypeCount;
    auto currentIsObject      = tag < Object::EClassTypeCount;
    auto parentIsTransform    = parentTag == ETransform;
    auto currentIsTransformOp = tag == ETranslate || tag == ERotate || tag == EScale || tag == ELookAt || tag == EMatrix;

    if (!hasParent && !currentIsObject)
      throw Exception(
        "Error while parsing \"%s\": root element \"%s\" must be a Object (at %s)",
        filename, node.name(), offset(node.offset_debug())
      );

    if (parentIsTransform != currentIsTransformOp)
      throw Exception(
        "Error while parsing \"%s\": transform nodes can only contain transform operations (at %s)",
        filename,  offset(node.offset_debug())
      );

    if (hasParent && !parentIsObject && !(parentIsTransform && currentIsTransformOp))
      throw Exception(
        "Error while parsing \"%s\": node \"%s\" requires a Object as parent (at %s)",
        filename, node.name(), offset(node.offset_debug())
      );

    if (tag == EScene)
      node.append_attribute("type") = "scene";
    else if (tag == ETransform)
      transform.setIdentity();

    PropertyList props;
    std::vector<Object*> children;
    for (auto& childNode : node.children()) {
      auto child = parseTag(childNode, props, tag);
      if (child)
        children.push_back(child);
    }

    Object* result = nullptr;


  };
}

}
