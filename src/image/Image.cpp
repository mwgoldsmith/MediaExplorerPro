/*****************************************************************************
* Media Explorer
*****************************************************************************/

#include "stdafx.h"
#if HAVE_CONFIG_H
# include "config.h"
#endif

#include "av/FrameBuffer.h"
#include "image/Image.h"
#include "logging/Logger.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
}

mxp::Image::Image(uint32_t x, uint32_t y, uint32_t c) noexcept
  : m_width{ x }
  , m_height{ y }
  , m_depth{ 1 }
  , m_channels{ c }
  , m_data{ nullptr }
  , m_size{ 0 }  {
}

mxp::Image::Image(uint32_t x, uint32_t y, uint32_t c, uint8_t* data, size_t size) noexcept
  : m_width{ x }
  , m_height{ y }
  , m_depth{ 1 }
  , m_channels{ c }
  , m_data{ nullptr }
  , m_size{ 0 } {
  Assign(data, c);
}

mxp::Image::Image(const Image& obj)
  : m_width{ 0 }
  , m_height{ 0 }
  , m_depth{ 0 }
  , m_channels{ 0}
  , m_data{ nullptr }
  , m_size{ 0 } {
  m_data = new uint8_t[obj.m_size];
  if(m_data != nullptr) {
    memcpy(m_data, obj.m_data, obj.m_size * sizeof(uint8_t));
    m_size = obj.m_size;
    m_width = obj.m_width;
    m_height = obj.m_height;
    m_depth = obj.m_depth;
    m_channels = obj.m_channels;
  }
}

mxp::Image::Image(Image&& obj) noexcept
  : m_width{ std::move(obj.m_width) }
  , m_height{ std::move(obj.m_height) }
  , m_depth{ std::move(obj.m_depth) }
  , m_channels{ std::move(obj.m_channels) }
  , m_data{ std::move(obj.m_data) }
  , m_size{ std::move(obj.m_size) } {
  obj.m_data = nullptr;
  obj.m_size = 0;
}

mxp::Image& mxp::Image::operator=(const Image& rhs) {
  auto temp{ rhs };
  std::swap(temp, *this);
  return *this;
}

mxp::Image& mxp::Image::operator=(Image&& obj) noexcept {
  std::swap(m_data, obj.m_data);
  std::swap(m_size, obj.m_size);
  std::swap(m_width, obj.m_width);
  std::swap(m_height, obj.m_height);
  std::swap(m_depth, obj.m_depth);
  std::swap(m_channels, obj.m_channels);

  return *this;
}

mxp::Image::~Image() {
  if (m_data != nullptr) {
    delete[] m_data;
  }
}


uint8_t mxp::Image::operator[](size_t index) const {
  return m_data[index];
}

const uint8_t* mxp::Image::GetData() const noexcept {
  return m_data;
}

uint32_t mxp::Image::GetX() const noexcept {
  return m_width;
}

uint32_t mxp::Image::GetY() const noexcept {
  return m_height;
}

uint32_t mxp::Image::GetZ() const noexcept {
  return m_depth;
}

uint32_t mxp::Image::GetChannels() const noexcept {
  return m_channels;
}

size_t mxp::Image::GetSize() const noexcept {
  return m_size;
}

bool mxp::Image::Assign(const uint8_t* data, size_t c) noexcept {
  auto success = true;

  try {
    m_data = new uint8_t[c];//static_cast<uint8_t*>(malloc(c * sizeof(uint8_t)));
    m_size = c;
    memcpy(m_data, data, c * sizeof(uint8_t));
  } catch(std::exception& ex) {
    LOG_ERROR("failed to assign Image: ", ex.what());
    m_data = {};
    m_size = 0;
    success = false;
  }

  return success;
}

bool mxp::Image::Scale(uint32_t x, uint32_t y, bool preserveRatio) {
  // Adjust the height or width to preserve the aspect ratio (if needed)
  if(preserveRatio) {
    auto ar = static_cast<double>(m_width) / static_cast<double>(m_height);
    if(max(ar, (double)x / (double)y) == ar) {
      y = static_cast<uint32_t>(x / ar);
    } else {
      x = static_cast<uint32_t>(y * ar);
    }
  }

  // Allocate source and destination frames
  auto frameSrc = FrameBuffer::Create(m_width, m_height);
  if(frameSrc == nullptr) {
    return false;
  }

  auto frameDst = FrameBuffer::Create(x, y);
  if(frameDst == nullptr) {
    return false;
  }

  auto pFrameSrc = frameSrc->GetFrame();
  auto pFrameDst = frameDst->GetFrame();
  auto data = m_data;
  const int in_linesize[1] = { 3 * pFrameSrc->width };
  const auto fmt = AV_PIX_FMT_RGB24;

  // Scale the image
  auto swc = sws_getContext(pFrameSrc->width, pFrameSrc->height, fmt, pFrameSrc->width, pFrameSrc->height, fmt, 0, NULL, NULL, NULL);
  sws_scale(swc, &data, in_linesize, 0, pFrameSrc->height, pFrameSrc->data, pFrameSrc->linesize);

  swc = sws_getCachedContext(swc, pFrameSrc->width, pFrameSrc->height, fmt, pFrameDst->width, pFrameDst->height, fmt, SWS_BICUBIC, NULL, NULL, NULL);
  sws_scale(swc, pFrameSrc->data, pFrameSrc->linesize, 0, pFrameSrc->height, pFrameDst->data, pFrameDst->linesize);

  // Assign the scaled image data to the new Image instance
  auto size = av_image_get_buffer_size(PIX_FMT_RGB24, x, y, 1);
  auto success = Assign(pFrameDst->data[0], size);
  if (success) {
    m_width = x;
    m_height = y;
  }

  sws_freeContext(swc);

  return success;
}

void mxp::Image::RotateImage(int theta, Image& oldImage) {
  int r0, c0;
  int r1, c1;
  int rows, cols;
  rows = oldImage.m_height;
  cols = oldImage.m_width;
  Image tempImage(cols, rows, oldImage.m_channels);

  auto rads = static_cast<float>((theta * 3.14159265) / 180.0); //converts the degree given by user into radians

  //find midpoints
  r0 = rows / 2;
  c0 = cols / 2;

  // goes through the array of the oldImage, uses the formulas to find where the pixel should go
  //  then puts the old pixel value into the new pixel position on the tempImage
  for(int r = 0; r < rows; r++) {
    for(int c = 0; c < cols; c++) {
      r1 = (int)(r0 + ((r - r0) * cos(rads)) - ((c - c0) * sin(rads)));
      c1 = (int)(c0 + ((r - r0) * sin(rads)) + ((c - c0) * cos(rads)));

      tempImage.m_data[GetOffset(c1, r1)] = oldImage.m_data[GetOffset(c, r)];

    }
  }

  for(int i = 0; i < rows; i++) {
    for(int j = 0; j < cols; j++) {
      if(tempImage.m_data[GetOffset(j, i)] == 0)
        tempImage.m_data[GetOffset(j, i)] = tempImage.m_data[GetOffset(j + 1, i)];
    }
  }
  oldImage = tempImage;
}


static uint32_t GetMax(uint32_t a, uint32_t b) {
  return (a >= b ? a : b);
}

mxp::HistogramPtr mxp::Image::CreateHistogram() const {
  auto hist = std::make_shared<Histogram<uint8_t>>();

  for(uint32_t i = 0; i < m_height; ++i) {
    auto pixelIndex = i * (3 * m_width);
    for(uint32_t j = 0; j <m_width * 3; j += 3) {
      ++hist->r[m_data[pixelIndex + j]];
      ++hist->g[m_data[pixelIndex + j + 1]];
      ++hist->b[m_data[pixelIndex + j + 2]];
    }
  }

  return hist;
}
