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
    ETexture,
    EClassTypeCount
  };

  virtual ~Object() = default;

  void setName(const std::string& name) {
    this->name = name;
  }

  std::string getName() const {
    return name;
  }

  virtual void addChild(Object* child) {
    throw Exception(
      "Object::addChild() is not implemented for objects of type '%s'!",
      classTypeName(getClassType())
    );
  }

  virtual void activate()
  { }

  virtual EClassType getClassType() const = 0;

  static std::string classTypeName(EClassType type) {
    switch (type) {
      case EScene:        return "scene";
      case EAccel:        return "accel";
      case EMesh:         return "mesh";
      case ECamera:       return "camera";
      case EIntegrator:   return "integrator";
      case ESampler:      return "sampler";
      case EBSDF:         return "bsdf";
      case ELight:        return "light";
      case EFilter:       return "filter";
      case ETexture:      return "texture";
      default:            return "<unknown>";
    }
  }

  virtual std::string toString() const = 0;

protected:
  std::string name;
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

#define MINPT_REGISTER_TEMPLATED_CLASS(cls, T, name)              \
  cls<T>* cls ##_## T ##_create(const PropertyList& props) {      \
    return new cls<T>(props);                                     \
  }                                                               \
  static struct cls ##_## T ##_ {                                 \
    cls ##_## T ##_() {                                           \
      ObjectFactory::registerClass(name, cls ##_## T ##_create);  \
    }                                                             \
  } cls ## T ##__MINPT_;

}
