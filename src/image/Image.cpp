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
  : m_data{}
  , m_x{ x }
  , m_y{ y }
  , m_z{ z }
  , m_size{ 0 }  {
}

mxp::Image::Image(uint32_t x, uint32_t y, uint32_t z, value_type* data, size_type c) noexcept
  : m_data{}
  , m_x{ x }
  , m_y{ y }
  , m_z{ z }
  , m_size{ 0 } {
  Assign(data, c);
}

mxp::Image::value_type mxp::Image::operator[](size_type index) {
  return (*m_data.get())[index];
}
std::shared_ptr<mxp::Image::buffer_type> mxp::Image::GetData() const noexcept {
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

mxp::Image::size_type mxp::Image::GetSize() const noexcept {
  return m_size;
}

bool mxp::Image::Assign(value_type* data, size_type c) noexcept {
  auto success = true;

  try {
    m_data = std::make_shared<buffer_type>(c);
    m_size = c;
    memcpy(m_data.get(), data, c * sizeof(value_type));
  } catch(std::exception& ex) {
    LOG_ERROR("failed to assign Image: ", ex.what());
    m_data = {};
    m_size = 0;
    success = false;
  }

  return success;
}
