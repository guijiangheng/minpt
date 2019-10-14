#include <set>
#include <vector>
#include <fstream>
#include <pugixml.hpp>
#include <Eigen/Geometry>

#include <minpt/utils/utils.h>
#include <minpt/core/object.h>
#include <minpt/core/parser.h>

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
    auto parentIsObject       = parentTag < (int)Object::EClassTypeCount;
    auto currentIsObject      = tag < (int)Object::EClassTypeCount;
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

    auto checkAttributes = [&](const pugi::xml_node& node, std::set<std::string> attrs) {
      for (auto& attr : node.attributes()) {
        auto it = attrs.find(attr.name());
        if (it == attrs.end())
          throw Exception(
            "Error while parsing \"%s\": unexpected attribute \"%s\" in \"%s\" at %s",
            filename, attr.name(), node.name(), offset(node.offset_debug())
          );
        attrs.erase(it);
      }
      if (!attrs.empty())
        throw Exception(
          "Error while parsing \"%s\": missing attribute \"%s\" in \"%s\" at %s",
          filename, *attrs.begin(), node.name(), offset(node.offset_debug())
        );
    };

    Object* result = nullptr;

    try {
      if (currentIsObject) {
        checkAttributes(node, { "type" });
        result = ObjectFactory::createInstance(node.attribute("type").value(), props);
        for (auto child : children)
          result->addChild(child);
        result->activate();
      } else {
        switch (tag) {
          case EBoolean:
            checkAttributes(node, { "name", "value" });
            parentProps.setBoolean(node.attribute("name").value(), toBool(node.attribute("value").value()));
            break;
          case EInteger:
            checkAttributes(node, { "name", "value" });
            parentProps.setInteger(node.attribute("name").value(), toInt(node.attribute("value").value()));
            break;
          case EFloat:
            checkAttributes(node, { "name", "value" });
            parentProps.setFloat(node.attribute("name").value(), toFloat(node.attribute("value").value()));
            break;
          case EString:
            checkAttributes(node, { "name", "value" });
            parentProps.setString(node.attribute("name").value(), node.attribute("value").value());
            break;
          case EVector:
            checkAttributes(node, { "name", "value" });
            parentProps.setVector(node.attribute("name").value(), toVector3f(node.attribute("value").value()));
            break;
          case EColor:
            checkAttributes(node, { "name", "value" });
            parentProps.setColor(node.attribute("name").value(), toColor3f(node.attribute("value").value()));
            break;
          case ETransform:
            checkAttributes(node, { "name" });
            parentProps.setTransform(node.attribute("name").value(), Matrix4f(transform.matrix()));
            break;
          case EMatrix: {
              checkAttributes(node, { "value" });
              auto tokens = tokenize(node.attribute("value").value());
              if (tokens.size() != 16)
                throw Exception("Expected 16 values");
              Eigen::Matrix4f matrix;
                for (int i = 0; i < 4; ++i)
                  for (auto j = 0; j < 4; ++j)
                    matrix(i, j) = toFloat(tokens[i * 4 + j]);
              transform = Eigen::Affine3f(matrix) * transform;
            }
            break;
          case EScale: {
              checkAttributes(node, { "value" });
              auto scale = toVector3f(node.attribute("value").value());
              transform = Eigen::DiagonalMatrix<float, 3>(scale) * transform;
            }
            break;
          case ERotate: {
              checkAttributes(node, { "angle", "axis" });
              auto angle = radians(toFloat(node.attribute("angle").value()));
              auto axis = toVector3f(node.attribute("axis").value());
              transform = Eigen::AngleAxis<float>(angle, axis) * transform;
            }
            break;
          case ELookAt:{
              checkAttributes(node, { "origin", "target", "up" });
              auto origin = toVector3f(node.attribute("origin").value());
              auto target = toVector3f(node.attribute("target").value());
              auto up = toVector3f(node.attribute("up").value());
              auto matrix = Matrix4f::lookAt(origin, target, up);
              transform = Eigen::Affine3f(matrix) * transform;
            }
            break;
          default:
            throw Exception("Unhandled element \"%s\"", node.name());
        }
      }
    } catch (const Exception& e) {
      throw Exception(
        "Error while parsing \"%s\": %s (at %s)",
        filename, e.what(), offset(node.offset_debug())
      );
    }

    return result;
  };

  PropertyList props;
  return parseTag(*doc.begin(), props, EInvalid);
}

}