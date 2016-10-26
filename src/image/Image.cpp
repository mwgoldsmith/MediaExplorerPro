/*****************************************************************************
* Media Explorer
*****************************************************************************/

#include "stdafx.h"
#if HAVE_CONFIG_H
# include "config.h"
#endif

#include "image/Image.h"
#include "logging/Logger.h"

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
