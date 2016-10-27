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

mxp::Image::Image(uint32_t x, uint32_t y, uint32_t z) noexcept
  : m_data{ nullptr }
  , m_x{ x }
  , m_y{ y }
  , m_z{ z }
  , m_size{ 0 }  {
}

mxp::Image::Image(uint32_t x, uint32_t y, uint32_t z, uint8_t* data, size_t c) noexcept
  : m_data{ nullptr }
  , m_x{ x }
  , m_y{ y }
  , m_z{ z }
  , m_size{ 0 } {
  Assign(data, c);
}

mxp::Image::~Image() {
  if (m_data != nullptr) {
    free(m_data);
  }
}

uint8_t mxp::Image::operator[](size_t index) {
  return m_data[index];
}

const uint8_t* mxp::Image::GetData() const noexcept {
  return m_data;
}

uint32_t mxp::Image::GetX() const noexcept {
  return m_x;
}

uint32_t mxp::Image::GetY() const noexcept {
  return m_y;
}

uint32_t mxp::Image::GetZ() const noexcept {
  return m_z;
}

size_t mxp::Image::GetSize() const noexcept {
  return m_size;
}

bool mxp::Image::Assign(const uint8_t* data, size_t c) noexcept {
  auto success = true;

  try {
    m_data = static_cast<uint8_t*>(malloc(c * sizeof(uint8_t)));
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
    auto ar = static_cast<double>(m_x) / static_cast<double>(m_y);
    if(max(ar, (double)x / (double)y) == ar) {
      y = static_cast<uint32_t>(x / ar);
    } else {
      x = static_cast<uint32_t>(y * ar);
    }
  }

  // Allocate source and destination frames
  auto frameSrc = FrameBuffer::Create(m_x, m_y);
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
    m_x = x;
    m_y = y;
  }

  sws_freeContext(swc);

  return success;
}

mxp::HistogramPtr mxp::Image::CreateHistogram() {
  auto hist = std::make_shared<Histogram<uint8_t>>();

  for(auto i = 0; i < m_y; ++i) {
    auto pixelIndex = i * (3 * m_x);
    for(auto j = 0; j <m_x* 3; j += 3) {
      ++hist->r[m_data[pixelIndex + j]];
      ++hist->g[m_data[pixelIndex + j + 1]];
      ++hist->b[m_data[pixelIndex + j + 2]];
    }
  }

  return hist;
}
