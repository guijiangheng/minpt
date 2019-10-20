#include <memory>

#include <lodepng.h>
#include <ImfInputFile.h>
#include <ImfOutputFile.h>
#include <ImfChannelList.h>
#include <ImfStringAttribute.h>
#include <ImfVersion.h>
#include <ImfIO.h>

#include <minpt/core/exception.h>
#include <minpt/utils/utils.h>
#include <minpt/utils/bitmap.h>

namespace minpt {

Bitmap::Bitmap(const std::string& filename) {
  filesystem::path path(filename);
  auto extension = path.extension();
  if (extension == "exr")
    readImageEXR(filename);
  else if (extension == "png" || extension == "jpg")
    readImagePNG(filename);
  else
    throw Exception(
      "Unable to read image file \"%s\", image format \"%s\" not supported!",
      filename, extension
    );
}

void Bitmap::readImagePNG(const std::string& filename) {
  unsigned width, height;
  std::vector<std::uint8_t> bytes;
  auto error = lodepng::decode(bytes, width, height, filename.c_str(), LCT_RGB);
  if (error)
    throw Exception(
      "Failed to load png file \"%s\": %s",
      filename, lodepng_error_text(error)
    );
  resize(height, width);
  auto offset = 0;
  for (auto y = 0u; y < height; ++y)
    for (auto x = 0u; x < width; ++x) {
      operator()(y, x) = Color3f(
        inverseGammaCorrect(bytes[offset]     / 255.0f),
        inverseGammaCorrect(bytes[offset + 1] / 255.0f),
        inverseGammaCorrect(bytes[offset + 2] / 255.0f)
      );
      offset += 3;
    }
}

void Bitmap::readImageEXR(const std::string& filename) {
  auto file = Imf::InputFile(filename.c_str());
  auto& header = file.header();
  auto& channels = header.channels();
  auto& window = header.dataWindow();

  resize(window.max.y - window.min.y + 1, window.max.x - window.min.x + 1);

  const char* r = nullptr;
  const char* g = nullptr;
  const char* b = nullptr;
  for (auto itr = channels.begin(); itr != channels.end(); ++itr) {
    // Sub-sampled layers are not supported
    if (itr.channel().xSampling != 1 || itr.channel().ySampling != 1) continue;
    auto name = toLower(itr.name());
    if (!r && (name == "r" || name == "red" || endsWith(name, ".r") || endsWith(name, ".red"))) r = itr.name();
    else if (!g && (name == "g" || name == "green" || endsWith(name, ".g") || endsWith(name, ".green"))) g = itr.name();
    else if (!b && (name == "b" || name == "blue" || endsWith(name, ".b") || endsWith(name, ".blue"))) b = itr.name();
  }

  if (!r || !g || !b)
    throw Exception("This is not a standard RGB OpenEXR file!");

  size_t compStride = sizeof(float);
  size_t pixelStride = compStride * 3;
  size_t rowStride = pixelStride * cols();
  auto bytes = reinterpret_cast<char*>(data());

  Imf::FrameBuffer frameBuffer;
  frameBuffer.insert(r, Imf::Slice(Imf::FLOAT, bytes, pixelStride, rowStride)); bytes += compStride;
  frameBuffer.insert(g, Imf::Slice(Imf::FLOAT, bytes, pixelStride, rowStride)); bytes += compStride;
  frameBuffer.insert(b, Imf::Slice(Imf::FLOAT, bytes, pixelStride, rowStride));
  file.setFrameBuffer(frameBuffer);
  file.readPixels(window.min.x, window.max.y);
}

void Bitmap::save(const std::string& filename) {
  filesystem::path path(filename);
  auto extension = path.extension();
  if (extension == "exr")
    writeImageEXR(filename);
  else if (extension == "png" || extension == "jpg")
    writeImagePNG(filename);
  else
    throw Exception(
      "Unable to write image file \"%s\", image format \"%s\" not supported!",
      filename, extension
    );
}

void Bitmap::writeImagePNG(const std::string& filename) const {
  unsigned width = cols();
  unsigned height = rows();
  std::unique_ptr<std::uint8_t[]> bytes(new std::uint8_t[width * height * 3]);
  auto offset = 0;
  for (auto y = 0u; y < height; ++y)
    for (auto x = 0u; x < width; ++x) {
      Color3f color = gammaCorrect(operator()(y, x));
      bytes[offset]     = (std::uint8_t)(std::min(color[0] * 255, 255.0f));
      bytes[offset + 1] = (std::uint8_t)(std::min(color[1] * 255, 255.0f));
      bytes[offset + 2] = (std::uint8_t)(std::min(color[2] * 255, 255.0f));
      offset += 3;
    }
  lodepng::encode(filename, bytes.get(), width, height, LCT_RGB);
}

void Bitmap::writeImageEXR(const std::string& filename) const {
  Imf::Header header((int)cols(), (int)(rows()));

  auto& channels = header.channels();
  channels.insert("R", Imf::Channel(Imf::FLOAT));
  channels.insert("G", Imf::Channel(Imf::FLOAT));
  channels.insert("B", Imf::Channel(Imf::FLOAT));

  Imf::FrameBuffer frameBuffer;
  size_t compStride = sizeof(float);
  size_t pixelStride = compStride * 3;
  size_t rowStride = pixelStride * cols();
  auto bytes = const_cast<char*>(reinterpret_cast<const char*>(data()));
  frameBuffer.insert("R", Imf::Slice(Imf::FLOAT, bytes, pixelStride, rowStride)); bytes += compStride;
  frameBuffer.insert("G", Imf::Slice(Imf::FLOAT, bytes, pixelStride, rowStride)); bytes += compStride;
  frameBuffer.insert("B", Imf::Slice(Imf::FLOAT, bytes, pixelStride, rowStride));

  Imf::OutputFile file(filename.c_str(), header);
  file.setFrameBuffer(frameBuffer);
  file.writePixels((int)rows());
}

}
