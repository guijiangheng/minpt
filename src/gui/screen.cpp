#include <minpt/gui/screen.h>

using namespace nanogui;

namespace minpt {

Screen::Screen(const ImageBlock& block)
    : nanogui::Screen(Eigen::Vector2i(block.size.x, block.size.y), "minpt", false)
    , block(block) {

  shader.init(
    "Tonemapper",

    "#version 330\n"
    "in vec2 position;\n"
    "out vec2 uv;\n"
    "void main() {\n"
    "  gl_Position = vec4(position.x * 2 - 1.0f, position.y * 2 - 1.0f, 0.0f, 1.0f);\n"
    "  uv = vec2(position.x, 1 - position.y);\n"
    "}",

    "#version 330\n"
    "uniform sampler2D source;\n"
    "in vec2 uv;\n"
    "out vec4 out_color;\n"
    "float toSRGB(float value) {\n"
    "  if (value < 0.0031308)\n"
    "    return 12.92f * value;\n"
    "  return 1.055f * pow(value, 0.41666f) - 0.055f;\n"
    "}\n"
    "void main() {\n"
    "  vec4 color = texture(source, uv);\n"
    "  color /= color.w;\n"
    "  out_color = vec4(toSRGB(color.r), toSRGB(color.g), toSRGB(color.b), 1.0f);\n"
    "}"
  );

  MatrixXu indices(3, 2);
  MatrixXf positions(2, 4);
  indices <<
    0, 2,
    1, 3,
    2, 0;
  positions <<
    0, 1, 1, 0,
    0, 0, 1, 1;

  shader.bind();
  shader.uploadAttrib("position", positions);
  shader.uploadIndices(indices);

  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

  drawAll();
  setVisible(true);
}

void Screen::drawContents() {
  auto& size = block.size;
  auto borderSize = block.borderSize;
  glBindTexture(GL_TEXTURE_2D, texture);
  glPixelStorei(GL_UNPACK_ROW_LENGTH, (GLuint)block.cols());
  block.lock();
  glTexImage2D(
    GL_TEXTURE_2D, 0, GL_RGBA32F, size.x, size.y, 0, GL_RGBA, GL_FLOAT,
    (uint8_t*)block.data() + (borderSize * block.cols() + borderSize) * sizeof(Color4f));
  block.unlock();

  glDisable(GL_DEPTH_TEST);
  glActiveTexture(GL_TEXTURE0);
  shader.bind();
  shader.setUniform("source", 0);
  shader.drawIndexed(GL_TRIANGLES, 0, 2);
  glEnable(GL_DEPTH_TEST);
}

}
