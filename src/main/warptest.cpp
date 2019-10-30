
#include <pcg32.h>
#include <nanogui/glutil.h>
#include <nanogui/layout.h>
#include <nanogui/screen.h>
#include <nanogui/window.h>
#include <nanogui/label.h>
#include <nanogui/slider.h>
#include <nanogui/textbox.h>
#include <nanogui/combobox.h>
#include <nanogui/checkbox.h>
#include <nanogui/messagedialog.h>

#include <minpt/core/sampling.h>
#include <minpt/core/exception.h>

using namespace nanogui;

class WarpTest : public Screen {
public:
  enum PointType {
    Independent = 0,
    Grid,
    Stratified
  };

  enum WarpType {
    None = 0,
    Disk,
    CosineHemisphere
  };

  WarpTest(): Screen(Vector2i(800, 600), "Sampling and Warping") {
    initializeGUI();
  }

  ~WarpTest() {
    delete gridShader;
    delete pointShader;
  }

  void runTest() {

  }

  std::pair<Vector3f, float> warpPoint(WarpType warpType, const Vector2f& point) {
    Vector3f result;
    minpt::Vector2f sample(point.x(), point.y());

    switch (warpType) {
      case None:
        result << point, 0.0f;
        break;
      case Disk: {
          auto temp = minpt::uniformSampleDisk(sample);
          result << temp.x, temp.y, 0.0f;
        }
        break;
      case CosineHemisphere: {
          auto temp = minpt::cosineSampleHemisphere(sample);
          result << temp.x, temp.y, temp.z;
        }
        break;
    }

    return std::make_pair(result, 1.0f);
  }

  void generatePoints(int& pointCount, PointType pointType, WarpType warpType, MatrixXf& positions, MatrixXf& weights) {
    auto sqrtValue = (int)(std::sqrt((float)pointCount) + 0.5f);
    auto invSqrtValue = 1.0f / sqrtValue;

    if (pointType == Grid || pointType == Stratified)
      pointCount = sqrtValue * sqrtValue;

    pcg32 rng;
    positions.resize(3, pointCount);
    weights.resize(1, pointCount);

    for (auto i = 0; i < pointCount; ++i) {
      Vector2f sample;
      auto y = i / sqrtValue;
      auto x = i % sqrtValue;

      switch (pointType) {
        case Independent:
          sample = Vector2f(rng.nextFloat(), rng.nextFloat());
          break;
        case Grid:
          sample = Vector2f((x + 0.5f) * invSqrtValue, (y + 0.5f) * invSqrtValue);
          break;
        case Stratified:
          sample = Vector2f(
            (x + rng.nextFloat()) * invSqrtValue,
            (y + rng.nextFloat()) * invSqrtValue
          );
          break;
      }

      auto result = warpPoint(warpType, sample);
      positions.col(i) = result.first;
      weights(0, i) = result.second;
    }
  }

  void refresh() {
    auto pointType = (PointType)pointTypeBox->selectedIndex();
    auto warpType = (WarpType)warpTypeBox->selectedIndex();
    pointCount = (int)std::pow(2.0f, 15 * pointCountSlider->value() + 5);

    MatrixXf positions, values;
    try {
      generatePoints(pointCount, pointType, warpType, positions, values);
    } catch (const minpt::Exception& e) {
      warpTypeBox->setSelectedIndex(0);
      refresh();
      new MessageDialog(this, MessageDialog::Type::Warning, "Error", "An error occurred: " + std::string(e.what()));
      return;
    }

    if (warpType != None) {
      for (auto i = 0; i < pointCount; ++i) {
        positions.col(i) = positions.col(i) * 0.5f + Vector3f(0.5f, 0.5f, 0.0f);
      }
    }

    // Generate a color gradient
    MatrixXf colors(3, pointCount);
    auto colorScale = 1.0f / pointCount;
    for (auto i = 0; i < pointCount; ++i)
      colors.col(i) << i * colorScale, 1 - i * colorScale, 0;

    pointShader->bind();
    pointShader->uploadAttrib("position", positions);
    pointShader->uploadAttrib("color", colors);

    if (gridCheckBox->checked()) {
      auto gridRes = (int)(std::sqrt((float)pointCount) + 0.5f);
      auto fineGridRes = gridRes * 15;
      lineCount = 4 * (gridRes + 1) * (fineGridRes + 1);
      positions.resize(3, lineCount);
      auto coarseScale = 1.0f / gridRes;
      auto fineScale = 1.0f / fineGridRes;

      auto index = 0;
      for (auto i = 0; i <= gridRes; ++i)
        for (auto j = 0; j <= fineGridRes; ++j) {
          positions.col(index++) = warpPoint(warpType, Vector2f(i * coarseScale, j * fineScale)).first;
          positions.col(index++) = warpPoint(warpType, Vector2f(i * coarseScale, (j + 1) * fineScale)).first;
          positions.col(index++) = warpPoint(warpType, Vector2f(j * fineScale, i * coarseScale)).first;
          positions.col(index++) = warpPoint(warpType, Vector2f((j + 1) * fineScale, i * coarseScale)).first;
        }

      if (warpType != None) {
        for (auto i = 0; i < lineCount; ++i)
          positions.col(i) = positions.col(i) * 0.5f + Vector3f(0.5f, 0.5f, 0.0f);
      }

      gridShader->bind();
      gridShader->uploadAttrib("position", positions);
    }

    // Update user interface
    std::string str;
    if (pointCount > 1000000) {
      pointCountBox->setUnits("M");
      str = tfm::format("%.2f", pointCount * 1e-6f);
    } else if (pointCount > 1000) {
      pointCountBox->setUnits("K");
      str = tfm::format("%.2f", pointCount * 1e-3f);
    } else {
      pointCountBox->setUnits(" ");
      str = tfm::format("%i", pointCount);
    }

    pointCountBox->setValue(str);
    pointCountSlider->setValue((std::log((float)pointCount) / std::log(2.f) - 5) / 15);
  }

  void drawContents() override {
    constexpr float viewAngle = 30.0f, near = 0.01f, far = 100.0f;
    auto fH = std::tan(viewAngle / 360.0f * M_PI) * near;
    auto fW = fH * mSize.x() / mSize.y();

    Matrix4f view = lookAt(Vector3f(0, 0, 2), Vector3f(0, 0, 0), Vector3f(0, 1, 0));
    Matrix4f project = frustum(-fW, fW, -fH, fH, near, far);
    Matrix4f model = arcball.matrix() * translate(Vector3f(-0.5f, -0.5f, 0.0f));
    Matrix4f mvp = project * view * model;

    pointShader->bind();
    pointShader->setUniform("mvp", mvp);
    glPointSize(2);
    glEnable(GL_DEPTH_TEST);
    pointShader->drawArray(GL_POINTS, 0, pointCount);

    if (gridCheckBox->checked()) {
      gridShader->bind();
      gridShader->setUniform("mvp", mvp);
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      gridShader->drawArray(GL_LINES, 0, lineCount);
      glDisable(GL_BLEND);
    }
  }

  void initializeGUI() {
    window = new Window(this, "warp tester");
    window->setLayout(new GroupLayout());

    new Label(window, "Input point set", "sans-bold");

    auto panel = new Widget(window);
    panel->setLayout(new BoxLayout(Orientation::Horizontal, Alignment::Middle, 0, 20));

    pointCountSlider = new Slider(panel);
    pointCountSlider->setFixedWidth(55);
    pointCountSlider->setCallback([=](float) { refresh(); });

    pointCountBox = new TextBox(panel);
    pointCountBox->setFixedSize(Eigen::Vector2i(80, 25));

    pointTypeBox = new ComboBox(window, { "Independent", "Grid", "Stratified" });
    pointTypeBox->setCallback([=](int) { refresh(); });

    new Label(window, "Warping method", "sans-bold");

    warpTypeBox = new ComboBox(window, { "None", "Disk", "Hemisphere (cos)" });
    warpTypeBox->setCallback([=](int) { refresh(); });

    panel = new Widget(window);
    panel->setLayout(new BoxLayout(Orientation::Horizontal, Alignment::Middle, 0, 20));

    gridCheckBox = new CheckBox(window, "Visualize warped grid");
    gridCheckBox->setCallback([=](bool) { refresh(); });

    new Label(window, "BSDF parameters", "sans-bold");

    panel = new Widget(window);
    panel->setLayout(new BoxLayout(Orientation::Horizontal, Alignment::Middle, 0, 20));

    angleSlider = new Slider(panel);
    angleSlider->setFixedWidth(55);
    angleSlider->setCallback([=](float) { refresh(); });

    angleBox = new TextBox(panel);
    angleBox->setFixedSize(Eigen::Vector2i(80, 25));
    angleBox->setUnits(utf8(0x00B0).data());

    brdfValueCheckBox = new CheckBox(window, "Visualize BRDF values");;
    brdfValueCheckBox->setCallback([=](bool) { refresh(); });

    new Label(
      window,
      std::string(utf8(0x03C7).data()) +
      std::string(utf8(0x00B2).data()) + " hypothesis test",
      "sans-bold"
    );

    auto button = new Button(window, "Run", ENTYPO_ICON_CHECK);
    button->setBackgroundColor(Color(0, 255, 0, 25));
    button->setCallback([=]{
      try {
        runTest();
      } catch (const minpt::Exception& e) {
        new MessageDialog(this, MessageDialog::Type::Warning, "Error", "An error occurred: " + std::string(e.what()));
      }
    });

    performLayout(mNVGContext);

    pointShader = new GLShader();
    pointShader->init(
      "Point shader",

      "#version 330\n"
      "uniform mat4 mvp;\n"
      "in vec3 position;\n"
      "in vec3 color;\n"
      "out vec3 frag_color;\n"
      "void main() {\n"
      "  gl_Position = mvp * vec4(position, 1.0);\n"
      "  frag_color = color;\n"
      "}",

      "#version 330\n"
      "in vec3 frag_color;\n"
      "out vec4 out_color;\n"
      "void main() {\n"
      "  out_color = vec4(frag_color, 1.0);\n"
      "}"
    );

    gridShader = new GLShader();
    gridShader->init(
      "Grid Shader",

      "#version 330\n"
      "uniform mat4 mvp;\n"
      "in vec3 position;\n"
      "void main() {\n"
      "  gl_Position = mvp * vec4(position, 1.0);\n"
      "}",

      "#version 330\n"
      "out vec4 out_color;\n"
      "void main() {\n"
      "  out_color = vec4(vec3(1.0), 0.4);\n"
      "}"
    );

    mBackground.setZero();
    pointCountSlider->setValue(0.5f);

    resizeEvent(mSize);
    refresh();
    setVisible(true);
  }

  bool resizeEvent(const Vector2i& size) override {
    if (!Screen::resizeEvent(size))
      arcball.setSize(mSize);
    return true;
  }

  bool mouseMotionEvent(const Vector2i& p, const Vector2i& rel, int button, int modifiers) override {
    if (!Screen::mouseMotionEvent(p, rel, button, modifiers))
      arcball.motion(p);
    return true;
  }

  bool mouseButtonEvent(const Vector2i& p, int button, bool down, int modifiers) override {
    if (!Screen::mouseButtonEvent(p, button, down, modifiers))
      if (button == GLFW_MOUSE_BUTTON_1)
        arcball.button(p, down);
    return true;
  }

private:
  int lineCount;
  int pointCount;
  Arcball arcball;
  Window* window;
  Slider* pointCountSlider;
  TextBox* pointCountBox;
  ComboBox* pointTypeBox;
  ComboBox* warpTypeBox;
  CheckBox* gridCheckBox;
  Slider* angleSlider;
  TextBox* angleBox;
  CheckBox* brdfValueCheckBox;
  GLShader* gridShader;
  GLShader* pointShader;
};

int main() {
  nanogui::init();
  auto screen = new WarpTest();
  nanogui::mainloop();
  delete screen;
  nanogui::shutdown();

  return 0;
}
