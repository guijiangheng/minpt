#pragma once

#include <nanogui/glutil.h>
#include <nanogui/screen.h>
#include <minpt/core/block.h>

namespace minpt {

class Screen : public nanogui::Screen {
public:
  Screen(const ImageBlock& block);

  ~Screen() {
    glDeleteTextures(1, &texture);
  }

  void drawContents() override;

private:
  const ImageBlock& block;
  nanogui::GLShader shader;
  GLuint texture = 0;
};

}
