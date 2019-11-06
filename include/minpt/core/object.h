#pragma once

#include <minpt/core/proplist.h>

namespace minpt {

class Object {
public:
  enum EClassType {
    EScene = 0,
    EAccel,
    EMesh,
    ECamera,
    EIntegrator,
    ESampler,
    EBSDF,
    ELight,
    EFilter,
    EClassTypeCount
  };

  virtual ~Object() = default;

  virtual void addChild(Object* child) {
    throw Exception(
      "Object::addChild() is not implemented for objects of type '%s'!",
      classTypeName(getClassType())
    );
  }

  virtual void activate()
  { }

  virtual EClassType getClassType() const = 0;

  virtual std::string toString() const = 0;

  static std::string classTypeName(EClassType type) {
    switch (type) {
      case EScene:      return "scene";
      case EAccel:      return "accel";
      case EMesh:       return "mesh";
      case ECamera:     return "camera";
      case EIntegrator: return "integrator";
      case ESampler:    return "sampler";
      case EBSDF:       return "bsdf";
      case ELight:      return "light";
      case EFilter:     return "filter";
      default:          return "<unknown>";
    }
  }
};

class ObjectFactory {
public:
  using Constructor = std::function<Object*(const PropertyList& props)>;

  static void registerClass(const std::string& name, const Constructor& constructor) {
    if (!constructors)
      constructors = new std::map<std::string, Constructor>();
    (*constructors)[name] = constructor;
  }

  static Object* createInstance(const std::string& name, const PropertyList& props) {
    if (constructors->find(name) == constructors->end())
      throw Exception("A constructor for class \"%s\" could not be found!", name);
    return (*constructors)[name](props);
  }

private:
  static std::map<std::string, Constructor>* constructors;
};

#define MINPT_REGISTER_CLASS(cls, name)                     \
  cls* cls ##_create(const PropertyList& props) {           \
    return new cls(props);                                  \
  }                                                         \
  static struct cls ##_ {                                   \
    cls ##_() {                                             \
      ObjectFactory::registerClass(name, cls ##_create);    \
    }                                                       \
  } cls ##_MINPT_;

}
