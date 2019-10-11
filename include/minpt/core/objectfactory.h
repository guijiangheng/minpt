#pragma once

#include <map>
#include <minpt/core/object.h>
#include <minpt/core/proplist.h>

namespace minpt {

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
