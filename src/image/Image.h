/*****************************************************************************
* Media Explorer
*****************************************************************************/

#ifndef IMAGE_H
#define IMAGE_H

#include "mediaexplorer/Common.h"

namespace mxp {
class Image {
public:
  using value_type      = uint8_t;
  using buffer_type     = std::vector<value_type>;
  using buffer_type_ptr = std::shared_ptr<buffer_type>;
  using size_type       = buffer_type::size_type;

  // Constructors / destructors
  Image(uint32_t x, uint32_t y, uint32_t z) noexcept;
  Image(uint32_t x, uint32_t y, uint32_t z, value_type* data, size_type c) noexcept;
  ~Image() = default;

  // Operator overloads:
  value_type operator[](size_type index);

  // Getters:
  std::shared_ptr<buffer_type> GetData() const noexcept;

  uint32_t GetX() const noexcept;

  uint32_t GetY() const noexcept;

  uint32_t GetZ() const noexcept;

  size_t GetSize() const noexcept;

  // Functions:
  bool Assign(value_type* data, size_type c) noexcept;

private:
  buffer_type_ptr m_data;
  uint32_t        m_x;
  uint32_t        m_y;
  uint32_t        m_z;
  size_type       m_size;
};

} /* namespace mxp */

#endif /* IMAGE_H */
