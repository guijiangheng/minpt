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
  Imf::Header header((int)cols(), (int)(rows()));

  auto& channels = header.channels();
  channels.insert("R", Imf::Channel(Imf::FLOAT));
  channels.insert("G", Imf::Channel(Imf::FLOAT));
  channels.insert("B", Imf::Channel(Imf::FLOAT));

  Imf::FrameBuffer frameBuffer;
  size_t compStride = sizeof(float);
  size_t pixelStride = compStride * 3;
  size_t rowStride = pixelStride * cols();
  auto bytes = reinterpret_cast<char*>(data());
  frameBuffer.insert("R", Imf::Slice(Imf::FLOAT, bytes, pixelStride, rowStride)); bytes += compStride;
  frameBuffer.insert("G", Imf::Slice(Imf::FLOAT, bytes, pixelStride, rowStride)); bytes += compStride;
  frameBuffer.insert("B", Imf::Slice(Imf::FLOAT, bytes, pixelStride, rowStride));

  Imf::OutputFile file(filename.c_str(), header);
  file.setFrameBuffer(frameBuffer);
  file.writePixels((int)rows());
}

}
