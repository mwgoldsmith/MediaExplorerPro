/*****************************************************************************
* Media Explorer
*****************************************************************************/

#ifndef IMAGE_H
#define IMAGE_H

#include "mediaexplorer/Common.h"

namespace mxp {
class Image {
public:
  // Constructors / destructors
  Image(uint32_t x, uint32_t y, uint32_t z) noexcept;
  Image(uint32_t x, uint32_t y, uint32_t z, uint8_t* data, size_t c) noexcept;
  ~Image();

  // Operator overloads:
  uint8_t operator[](size_t index);

  // Getters:
  const uint8_t* GetData() const noexcept;

  uint32_t GetX() const noexcept;

  uint32_t GetY() const noexcept;

  uint32_t GetZ() const noexcept;

  size_t GetSize() const noexcept;

  // Functions:
  bool Assign(const uint8_t* data, size_t c) noexcept;

private:
  uint8_t*        m_data;
  uint32_t        m_x;
  uint32_t        m_y;
  uint32_t        m_z;
  size_t          m_size;
};

} /* namespace mxp */

#endif /* IMAGE_H */
