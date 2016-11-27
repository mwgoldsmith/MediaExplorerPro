/*****************************************************************************
* Media Explorer
*****************************************************************************/

#ifndef IIMAGECONTAINER_H
#define IIMAGECONTAINER_H

#include "mediaexplorer/Common.h"
#include "Types.h"

namespace mxp {

class IImageContainer {
public:
  virtual ~IImageContainer() = default;

  /**
   * @return The file name extension to use for the image container
   */
  virtual const mchar* GetExtension() const = 0;

  /**
   * @return The desired input fourcc for the image compressor
   */
  virtual const char* GetFourCC() const = 0;

  /**
   * @return The number of bits per pixel
   */
  virtual uint32_t GetBpp() const = 0;

  /**
   * @return The number of elements in the image data buffer.
   */
  virtual size_t GetSize() const = 0;

  /**
   * @brief Compresses the specified image and stores the data in memory 
   *
   * @param image The war image data to compress.
   *
   * @return If successful, true; otherwise, false.
   */
  virtual bool Compress(ImagePtr image) = 0;

  virtual bool Save(const mstring& filename) const = 0;
};

} /* namespace mxp */

#endif /* IIMAGECONTAINER_H */
