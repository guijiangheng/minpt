#pragma once

#include <minpt/core/mesh.h>

namespace minpt {

class WavefrontOBJ : public Mesh {
public:
  WavefrontOBJ(const std::string& filename);
};

}
