/*****************************************************************************
* Media Explorer
*****************************************************************************/

#ifndef IMAGE_H
#define IMAGE_H

#include "av/Histogram.h"
#include "mediaexplorer/Common.h"
#include "Types.h"

namespace mxp {

class Image {
public:
  // Constructors / destructors
  Image(uint32_t x, uint32_t y, uint32_t c) noexcept;
  Image(uint32_t x, uint32_t y, uint32_t c, uint8_t* data, size_t size) noexcept;
  Image(const Image &obj);
  Image(Image&&) noexcept;
  ~Image();

  // Operator overloads:
  Image& operator=(const Image& rhs);
  Image& operator=(Image&&) noexcept;
  uint8_t operator[](size_t index) const;

  // Getters:
  const uint8_t* GetData() const noexcept;

  uint32_t GetX() const noexcept;

  uint32_t GetY() const noexcept;

  uint32_t GetZ() const noexcept;

  uint32_t GetChannels() const noexcept;

  size_t GetSize() const noexcept;

  // Functions:
  bool Assign(const uint8_t* data, size_t c) noexcept;

  bool Scale(uint32_t x, uint32_t y, bool preserveRatio);

  void RotateImage(int theta, Image & oldImage);

  //bool Deriche(const float sigma, const int order, const char axis, const bool boundary_conditions);
  //bool Blur(const float sigma, const bool boundaryConditions = true, const bool isGaussian = false);

  HistogramPtr CreateHistogram() const;

private:
  inline long GetOffset(const int x, const int y = 0, const int z = 0, const int c = 0) const {
    return x + y*static_cast<long>(m_width) + z*(long)m_width*m_height + c*(long)m_width*m_height*m_depth;
  }

  uint32_t        m_width;
  uint32_t        m_height;
  uint32_t        m_depth;
  uint32_t        m_channels;
  uint8_t*        m_data;
  size_t          m_size;
};

} /* namespace mxp */

#endif /* IMAGE_H */
