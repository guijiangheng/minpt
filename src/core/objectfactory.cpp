#include <minpt/core/object.h>

namespace minpt {

std::map<std::string, ObjectFactory::Constructor> *ObjectFactory::constructors = nullptr;

}
