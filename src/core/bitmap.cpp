#include <minpt/core/bitmap.h>
#include <minpt/common/common.h>
#include <minpt/core/exception.h>
#include <ImfInputFile.h>
#include <ImfOutputFile.h>
#include <ImfChannelList.h>
#include <ImfStringAttribute.h>
#include <ImfVersion.h>
#include <ImfIO.h>

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
    std::string name = toLower(itr.name());
    if (!r && (name == "r" || name == "red" || endsWith(name, ".r") || endsWith(name, ".red"))) r = itr.name();
    else if (!g && (name == "g" || name == "green" || endsWith(name, ".g") || endsWith(name, ".green"))) g = itr.name();
    else if (!b && (name == "b" || name == "blue" || endsWith(name, ".b") || endsWith(name, ".blue"))) b = itr.name();
  }

  if (!r || !g || !b)
    throw Exception("This is not a standard RGB OpenEXR file!");

  auto compStride = sizeof(float);
  auto pixelStride = compStride * 3;
  auto rowStride = pixelStride * cols();
  auto bytes = reinterpret_cast<char*>(data());

  Imf::FrameBuffer frameBuffer;
  frameBuffer.insert(r, Imf::Slice(Imf::FLOAT, bytes, pixelStride, rowStride)); bytes += compStride;
  frameBuffer.insert(g, Imf::Slice(Imf::FLOAT, bytes, pixelStride, rowStride)); bytes += compStride;
  frameBuffer.insert(b, Imf::Slice(Imf::FLOAT, bytes, pixelStride, rowStride));
  file.setFrameBuffer(frameBuffer);
  file.readPixels(window.min.x, window.min.y);
}

}
