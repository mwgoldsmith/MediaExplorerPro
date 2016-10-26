/*****************************************************************************
* Media Explorer
*****************************************************************************/

#ifndef JPEGIMAGE_H
#define JPEGIMAGE_H

#include "IImageContainer.h"
#include "Types.h"

namespace mxp {

class JpegImage : public IImageContainer {
public:
  virtual const char* GetExtension() const override;
  virtual const char* GetFourCC() const override;
  virtual uint32_t bpp() const override;
  bool save(ImagePtr image, const std::string & outputFile);
  virtual bool compress(const uint8_t* buffer, const std::string& output,
    uint32_t inputWidth, uint32_t inputHeight,
    uint32_t outputWidth, uint32_t outputHeight,
    uint32_t hOffset, uint32_t vOffset) override;
};

} /* namespace mxp */

#endif /* JPEGIMAGE_H */
