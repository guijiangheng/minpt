#include <memory>

#include <pcg32.h>
#include <hypothesis.h>

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

#include <minpt/math/math.h>
#include <minpt/utils/bitmap.h>
#include <minpt/core/exception.h>
#include <minpt/core/sampling.h>
#include <minpt/core/distribution.h>
#include <minpt/microfacets/beckmann.h>

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
    Triangle,
    Disk,
    UniformHemisphere,
    CosineHemisphere,
    Beckmann,
    HDR
  };

  WarpTest()
      : Screen(Vector2i(800, 600), "Sampling and Warping")
      , hdrBitmap("../assets/lightprobe.exr") {

    auto data = std::make_unique<float[]>(hdrBitmap.size());
    auto width = (int)hdrBitmap.cols();
    auto height = (int)hdrBitmap.rows();
    for (auto y = 0; y < height; ++y)
      for (auto x = 0; x < width; ++x)
        data[y * width + x] = hdrBitmap(y, x).y();
    distrib = minpt::Distribution2D(data.get(), width, height);

    luminance = std::make_unique<double[]>(hdrBitmap.size());
    auto sumInv = 1.0f / distrib.pMarginal.getSum();
    for (auto y = 0; y < height; ++y)
      for (auto x = 0; x < width; ++x)
        luminance[y * width + x] = data[y * width + x] * sumInv;

    initializeGUI();
    shouldDrawHistogram = false;
  }

  ~WarpTest() {
    glDeleteTextures(1, &hdrTexture);
    glDeleteTextures(2, &textures[0]);
  }

  static float mapParameter(float parameter) {
    return std::exp(std::log(0.01f) * (1 - parameter) + std::log(1.0f) * parameter);
  }

  void runTest() {
    auto xres = 51;
    auto yres = 51;
    auto warpType = (WarpType)warpTypeBox->selectedIndex();
    auto parameterValue = mapParameter(parameterSlider->value());

    if (warpType != None && warpType != Triangle && warpType != Disk) xres *= 2;

    if (warpType == HDR) {
      xres = hdrBitmap.cols();
      yres = hdrBitmap.rows();
    }

    auto res = xres * yres;
    auto sampleCount = res * (warpType == HDR ? 100 : 1000);
    auto obsFrequencies = std::make_unique<double[]>(res);
    auto expFrequencies = std::make_unique<double[]>(res);
    std::memset(obsFrequencies.get(), 0, res * sizeof(double));
    std::memset(expFrequencies.get(), 0, res * sizeof(double));

    MatrixXf points, values;
    generatePoints(sampleCount, Independent, warpType, parameterValue, points, values);

    for (auto i = 0; i < sampleCount; ++i) {
      float x, y;
      Vector3f sample = points.col(i);
      if (warpType == None || warpType == Triangle) {
        x = sample.x();
        y = sample.y();
      } else if (warpType == HDR) {
        x = sample.x();
        y = 1.0f - sample.y();
      } else if (warpType == Disk) {
        x = sample.x() * 0.5f + 0.5f;
        y = sample.y() * 0.5f + 0.5f;
      } else {
        x = std::atan2(sample.y(), sample.x()) * minpt::Inv2Pi;
        if (x < 0) x += 1;
        y = sample.z() * 0.5f + 0.5f;
      }
      auto xbin = std::min(xres - 1, std::max(0, (int)std::floor(x * xres)));
      auto ybin = std::min(yres - 1, std::max(0, (int)std::floor(y * yres)));
      ++obsFrequencies[ybin * xres + xbin];
    }

    auto integrand = [&](double y, double x) -> double {
      if (warpType == None)
        return 1.0f;
      else if (warpType == Triangle)
        return minpt::uniformSampleTrianglePdf(minpt::Vector2f(x, y));
      else if (warpType == Disk) {
        x = x * 2 - 1;
        y = y * 2 - 1;
        return minpt::uniformSampleDiskPdf(minpt::Vector2f(x, y));
      } else {
        x *= 2 * minpt::Pi;
        y = y * 2 - 1;
        auto v = minpt::sphericalDirection(std::sqrt(1 - y * y), y, x);
        if (warpType == UniformHemisphere)
          return minpt::uniformSampleHemispherePdf(v);
        else if (warpType == CosineHemisphere)
          return minpt::cosineSampleHemispherePdf(v);
        else if (warpType == Beckmann)
          return v.z < 0 ? 0 : beckmannDistrib->pdf(v);
        else
          throw minpt::Exception("Invalid warp type");
      }
    };

    double scale = sampleCount;

    if (warpType == HDR) {
      for (auto y = 0; y < yres; ++y)
        for (auto x = 0; x < xres; ++x)
          expFrequencies[y * xres + x] = luminance[y * xres + x] * scale;
    } else {
      if (warpType == None || warpType == Triangle) scale *= 1;
      else if (warpType == Disk) scale *= 4;
      else scale *= 4 * minpt::Pi;

      constexpr auto Epsilon = 1e-4;
      for (auto y = 0; y < yres; ++y) {
        auto ybeg = y / (double)yres;
        auto yend = (y + 1 - Epsilon) / (double)yres;
        for (auto x = 0; x < xres; ++x) {
          auto xbeg = x / (double)xres;
          auto xend = (x + 1 - Epsilon) / (double)xres;
          expFrequencies[y * xres + x] = hypothesis::adaptiveSimpson2D(integrand, ybeg, xbeg, yend, xend) * scale;
          if (expFrequencies[y * xres + x] < 0)
            throw minpt::Exception("The Pdf() function returned negative values!");
        }
      }
    }

    /* Write the test input data to disk for debugging */
    hypothesis::chi2_dump(yres, xres, obsFrequencies.get(), expFrequencies.get(), "chitest.m");

    constexpr auto minExpFrequency = 5;
    constexpr auto significanceLevel = 0.01f;
    testResult = hypothesis::chi2_test(yres * xres, obsFrequencies.get(), expFrequencies.get(),
      sampleCount, minExpFrequency, significanceLevel, 1);

    float maxValue = 0, minValue = std::numeric_limits<float>::infinity();
    for (auto i = 0; i < res; ++i) {
      maxValue = std::max(maxValue, (float)std::max(obsFrequencies[i], expFrequencies[i]));
      minValue = std::min(minValue, (float)std::min(obsFrequencies[i], expFrequencies[i]));
    }
    minValue /= 2;
    auto texScale = 1 / (maxValue - minValue);

    auto buffer = std::make_unique<float[]>(res);
    for (auto k = 0; k < 2; ++k) {
      for (auto i = 0; i < res; ++i)
        buffer[i] = ((k == 0 ? obsFrequencies[i] : expFrequencies[i]) - minValue) * texScale;
      glBindTexture(GL_TEXTURE_2D, textures[k]);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, xres, yres,
                   0, GL_RED, GL_FLOAT, buffer.get());
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    }

    shouldDrawHistogram = true;
    window->setVisible(false);
  }

  std::pair<Vector3f, float> warpPoint(WarpType warpType, const Vector2f& point, float parameterValue) {
    Vector3f result;
    minpt::Vector2f u(point.x(), point.y());

    switch (warpType) {
      case None:
        result << point, 0.0f;
        break;
      case HDR: {
        float pdf;
        auto temp = distrib.sampleContinuous(u, pdf);
        result << temp.x, 1.0f - temp.y, 0.0f;
        break;
      }
      case Triangle: {
          auto temp = minpt::uniformSampleTriangle(u);
          result << temp.x, temp.y, 0.0f;
        }
        break;
      case Disk: {
          auto temp = minpt::uniformSampleDisk(u);
          result << temp.x, temp.y, 0.0f;
        }
        break;
      case UniformHemisphere: {
          auto temp = minpt::uniformSampleHemisphere(u);
          result << temp.x, temp.y, temp.z;
        }
        break;
      case CosineHemisphere: {
          auto temp = minpt::cosineSampleHemisphere(u);
          result << temp.x, temp.y, temp.z;
        }
        break;
      case Beckmann: {
        auto temp = beckmannDistrib->sample(u);
        result << temp.x, temp.y, temp.z;
      }
      break;
    }

    return std::make_pair(result, 1.0f);
  }

  void generatePoints(
      int& pointCount,
      PointType pointType,
      WarpType warpType,
      float parameterValue,
      MatrixXf& positions,
      MatrixXf& weights) {

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

      auto result = warpPoint(warpType, sample, parameterValue);
      positions.col(i) = result.first;
      weights(0, i) = result.second;
    }
  }

  void refresh() {
    auto pointType = (PointType)pointTypeBox->selectedIndex();
    auto warpType = (WarpType)warpTypeBox->selectedIndex();

    auto parameterValue = mapParameter(parameterSlider->value());
    pointCount = (int)std::pow(2.0f, 15 * pointCountSlider->value() + 5);

    if (warpType == HDR) {
      setSize(Vector2i(hdrBitmap.cols(), hdrBitmap.rows()));
      gridCheckBox->setEnabled(false);
      gridCheckBox->setChecked(false);
    } else {
      gridCheckBox->setEnabled(true);
    }

    if (warpType == Beckmann) {
      auto alpha = minpt::BeckmannDistribution::roughnessToAlpha(parameterValue);
      beckmannDistrib = std::make_unique<minpt::BeckmannDistribution>(alpha, alpha);
    }

    MatrixXf positions, values;
    try {
      generatePoints(pointCount, pointType, warpType, parameterValue, positions, values);
    } catch (const minpt::Exception& e) {
      warpTypeBox->setSelectedIndex(0);
      refresh();
      new MessageDialog(this, MessageDialog::Type::Warning, "Error", "An error occurred: " + std::string(e.what()));
      return;
    }

    if (warpType == HDR) {
      for (auto i = 0; i < pointCount; ++i)
        positions.col(i) = positions.col(i) * 2.0f - Vector3f(1.0f, 1.0f, 0.0f);
    } else if (warpType != None && warpType != Triangle) {
      for (auto i = 0; i < pointCount; ++i)
        positions.col(i) = positions.col(i) * 0.5f + Vector3f(0.5f, 0.5f, 0.0f);
    }

    // Generate a color gradient
    MatrixXf colors(3, pointCount);
    auto colorScale = 1.0f / pointCount;
    for (auto i = 0; i < pointCount; ++i)
      colors.col(i) << i * colorScale, 1 - i * colorScale, 0;

    pointShader.bind();
    pointShader.uploadAttrib("position", positions);
    pointShader.uploadAttrib("color", colors);

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
          positions.col(index++) = warpPoint(warpType, Vector2f(i * coarseScale, j * fineScale), parameterValue).first;
          positions.col(index++) = warpPoint(warpType, Vector2f(i * coarseScale, (j + 1) * fineScale), parameterValue).first;
          positions.col(index++) = warpPoint(warpType, Vector2f(j * fineScale, i * coarseScale), parameterValue).first;
          positions.col(index++) = warpPoint(warpType, Vector2f((j + 1) * fineScale, i * coarseScale), parameterValue).first;
        }

      if (warpType != None) {
        for (auto i = 0; i < lineCount; ++i)
          positions.col(i) = positions.col(i) * 0.5f + Vector3f(0.5f, 0.5f, 0.0f);
      }

      gridShader.bind();
      gridShader.uploadAttrib("position", positions);
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
    parameterBox->setValue(tfm::format("%.1g", parameterValue));
    parameterSlider->setEnabled(warpType == Beckmann);
  }

  void drawHistogram(const Vector2i& pos, const Vector2i& size_, GLuint tex) {
    Vector2f s = -(pos.array().cast<float>() + Vector2f(0.25f, 0.25f).array())  / size_.array().cast<float>();
    Vector2f e = size().array().cast<float>() / size_.array().cast<float>() + s.array();
    Matrix4f mvp = ortho(s.x(), e.x(), e.y(), s.y(), -1, 1);
    glDisable(GL_DEPTH_TEST);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex);
    histogramShader.bind();
    histogramShader.setUniform("mvp", mvp);
    histogramShader.setUniform("tex", 0);
    histogramShader.drawIndexed(GL_TRIANGLES, 0, 2);
  }

  void drawContents() override {
    if (shouldDrawHistogram) {
      auto warpType = (WarpType)warpTypeBox->selectedIndex();
      constexpr int spacer = 20;
      constexpr int lineHeight = (int)24 * 1.2f;
      const int histWidth = (width() - 3 * spacer) / 2;
      const int histHeight = (warpType == None || warpType == Triangle || warpType == Disk) ? histWidth : histWidth / 2;
      const int verticalOffset = (height() - histHeight - lineHeight - 70 - 2 * spacer) / 2;
      drawHistogram(Vector2i(spacer, verticalOffset + lineHeight + spacer), Vector2i(histWidth, histHeight), textures[0]);
      drawHistogram(Vector2i(2 * spacer + histWidth, verticalOffset + lineHeight + spacer), Vector2i(histWidth, histHeight), textures[1]);

      auto ctx = mNVGContext;
      nvgBeginFrame(ctx, mSize[0], mSize[1], mPixelRatio);
      nvgBeginPath(ctx);
      nvgRect(ctx, spacer, verticalOffset + lineHeight + histHeight + spacer * 2, width() - 2 * spacer, 70);
      nvgFillColor(ctx, testResult.first ? Color(100, 255, 100, 100) : Color(255, 100, 100, 100));
      nvgFill(ctx);
      nvgFontSize(ctx, 24.0f);
      nvgFontFace(ctx, "sans-bold");
      nvgTextAlign(ctx, NVG_ALIGN_CENTER | NVG_ALIGN_BOTTOM);
      nvgFillColor(ctx, Color(255, 255));
      nvgText(ctx, spacer + histWidth / 2, verticalOffset + lineHeight, "Sample histogram", nullptr);
      nvgText(ctx, 2 * spacer + histWidth * 1.5f, verticalOffset + lineHeight, "Integrated density", nullptr);
      nvgStrokeColor(ctx, Color(255, 255));
      nvgStrokeWidth(ctx, 2);
      nvgBeginPath(ctx);
      nvgRect(ctx, spacer, verticalOffset + lineHeight + spacer, histWidth, histHeight);
      nvgRect(ctx, 2 * spacer + histWidth, verticalOffset + lineHeight + spacer, histWidth, histHeight);
      nvgStroke(ctx);

      nvgFontSize(ctx, 20.0f);
      nvgTextAlign(ctx, NVG_ALIGN_CENTER | NVG_ALIGN_TOP);
      float bounds[4];
      nvgTextBoxBounds(ctx, 0, 0, width() - 2 * spacer, testResult.second.c_str(), nullptr, bounds);
      nvgTextBox(
        ctx, spacer, verticalOffset + histHeight + lineHeight + spacer * 2 + (70 - bounds[3]) / 2,
        width() - 2 * spacer, testResult.second.c_str(), nullptr);
      nvgEndFrame(ctx);
    } else {
      auto warpType = (WarpType)warpTypeBox->selectedIndex();

      if (warpType == HDR) {
        glDisable(GL_DEPTH_TEST);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, hdrTexture);
        hdrShader.bind();
        hdrShader.setUniform("source", 0);
        hdrShader.drawIndexed(GL_TRIANGLES, 0, 2);
        glEnable(GL_DEPTH_TEST);
      }

      constexpr float viewAngle = 30.0f, near = 0.01f, far = 100.0f;
      auto fH = std::tan(viewAngle / 360.0f * M_PI) * near;
      auto fW = fH * mSize.x() / mSize.y();
      Matrix4f view = lookAt(Vector3f(0, 0, 2), Vector3f(0, 0, 0), Vector3f(0, 1, 0));
      Matrix4f project = frustum(-fW, fW, -fH, fH, near, far);
      Matrix4f model = arcball.matrix() * translate(Vector3f(-0.5f, -0.5f, 0.0f));
      Matrix4f mvp = project * view * model;

      if (warpType == HDR)
        mvp = Matrix4f::Identity();

      pointShader.bind();
      pointShader.setUniform("mvp", mvp);
      glPointSize(2);
      glEnable(GL_DEPTH_TEST);
      pointShader.drawArray(GL_POINTS, 0, pointCount);

      if (gridCheckBox->checked()) {
        gridShader.bind();
        gridShader.setUniform("mvp", mvp);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        gridShader.drawArray(GL_LINES, 0, lineCount);
        glDisable(GL_BLEND);
      }
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

    warpTypeBox = new ComboBox(window, { "None", "Triangle", "Disk", "Hemisphere (uni)", "Hemisphere (cos)", "Beckmann", "HDR" });
    warpTypeBox->setCallback([=](int) { refresh(); });

    panel = new Widget(window);
    panel->setLayout(new BoxLayout(Orientation::Horizontal, Alignment::Middle, 0, 20));
    parameterSlider = new Slider(panel);
    parameterSlider->setFixedWidth(55);
    parameterSlider->setCallback([=](bool) { refresh(); });
    parameterBox = new TextBox(panel);
    parameterBox->setFixedSize(Vector2i(80, 25));

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

    brdfValueCheckBox = new CheckBox(window, "Visualize BRDF values");
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

    pointShader.init(
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

    gridShader.init(
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

    histogramShader.init(
      "Histogram shader",

      "#version 330\n"
      "uniform mat4 mvp;\n"
      "in vec2 position;\n"
      "out vec2 uv;\n"
      "void main() {\n"
      "  gl_Position = mvp * vec4(position, 0.0, 1.0);\n"
      "  uv = position;\n"
      "}",

      "#version 330\n"
      "out vec4 out_color;\n"
      "uniform sampler2D tex;\n"
      "in vec2 uv;\n"
      "/* http://paulbourke.net/texture_colour/colourspace/ */\n"
      "vec3 colormap(float v, float vmin, float vmax) {\n"
      "  vec3 c = vec3(1.0);\n"
      "  if (v < vmin)\n"
      "    v = vmin;\n"
      "  if (v > vmax)\n"
      "    v = vmax;\n"
      "  float dv = vmax - vmin;\n"
      "  \n"
      "  if (v < (vmin + 0.25 * dv)) {\n"
      "    c.r = 0.0;\n"
      "    c.g = 4.0 * (v - vmin) / dv;\n"
      "  } else if (v < (vmin + 0.5 * dv)) {\n"
      "    c.r = 0.0;\n"
      "    c.b = 1.0 + 4.0 * (vmin + 0.25 * dv - v) / dv;\n"
      "  } else if (v < (vmin + 0.75 * dv)) {\n"
      "    c.r = 4.0 * (v - vmin - 0.5 * dv) / dv;\n"
      "    c.b = 0.0;\n"
      "  } else {\n"
      "    c.g = 1.0 + 4.0 * (vmin + 0.75 * dv - v) / dv;\n"
      "    c.b = 0.0;\n"
      "  }\n"
      "  return c;\n"
      "}\n"
      "void main() {\n"
      "  float value = texture(tex, uv).r;\n"
      "  out_color = vec4(colormap(value, 0.0, 1.0), 1.0);\n"
      "}"
    );

    MatrixXf positions(2, 4);
    MatrixXu indices(3, 2);
    positions <<
      0, 1, 1, 0,
      0, 0, 1, 1;
    indices <<
      0, 2,
      1, 3,
      2, 0;
    histogramShader.bind();
    histogramShader.uploadAttrib("position", positions);
    histogramShader.uploadIndices(indices);

    glGenTextures(2, &textures[0]);

    hdrShader.init(
      "HDR Shader",

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

    hdrShader.bind();
    histogramShader.uploadAttrib("position", positions);
    histogramShader.uploadIndices(indices);

    glGenTextures(1, &hdrTexture);
    glBindTexture(GL_TEXTURE_2D, hdrTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, hdrBitmap.cols(), hdrBitmap.rows(), 0, GL_RGB, GL_FLOAT, (uint8_t*)hdrBitmap.data());

    mBackground.setZero();
    pointCountSlider->setValue(0.5f);
    parameterSlider->setValue(0.5f);

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
    if (down && !window->visible()) {
      shouldDrawHistogram = false;
      window->setVisible(true);
      return true;
    }
    if (!Screen::mouseButtonEvent(p, button, down, modifiers))
      if (button == GLFW_MOUSE_BUTTON_1)
        arcball.button(p, down);
    return true;
  }

private:
  bool shouldDrawHistogram;
  int lineCount;
  int pointCount;
  GLuint textures[2];
  GLuint hdrTexture;
  std::pair<bool, std::string> testResult;
  Arcball arcball;
  Window* window;
  Slider* pointCountSlider;
  TextBox* pointCountBox;
  Slider* parameterSlider;
  TextBox* parameterBox;
  ComboBox* pointTypeBox;
  ComboBox* warpTypeBox;
  CheckBox* gridCheckBox;
  Slider* angleSlider;
  TextBox* angleBox;
  CheckBox* brdfValueCheckBox;
  GLShader gridShader;
  GLShader pointShader;
  GLShader histogramShader;
  GLShader hdrShader;
  minpt::Bitmap hdrBitmap;
  minpt::Distribution2D distrib;
  std::unique_ptr<double[]> luminance;
  std::unique_ptr<minpt::MicrofacetDistribution> beckmannDistrib;
};

int main() {
  nanogui::init();
  auto screen = new WarpTest();
  nanogui::mainloop();
  delete screen;
  nanogui::shutdown();
  return 0;
}
