/*****************************************************************************
* Media Explorer
*****************************************************************************/

#ifndef IIMAGECONTAINER_H
#define IIMAGECONTAINER_H

#include "mediaexplorer/Common.h"

namespace mxp {

class IImageContainer {
public:
  virtual ~IImageContainer() = default;

  /**
   * @return 
   */
  virtual const mchar* GetExtension() const = 0;

  /**
   * @return Returns the desired input fourcc for this compressor
   */
  virtual const char* GetFourCC() const = 0;

  /**
   * @return
   */
  virtual uint32_t bpp() const = 0;

  virtual bool compress(const uint8_t* buffer, const std::string& output,
    uint32_t inputWidth, uint32_t inputHeight,
    uint32_t outputWidth, uint32_t outputHeight,
    uint32_t hOffset, uint32_t vOffset) = 0;
};

} /* namespace mxp */

#endif /* IIMAGECONTAINER_H */
