#include <minpt/core/objectfactory.h>

namespace minpt {

std::map<std::string, ObjectFactory::Constructor> *ObjectFactory::constructors = nullptr;

}
