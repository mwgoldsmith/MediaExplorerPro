/*****************************************************************************
* Media Explorer
*****************************************************************************/

#ifndef JPEGIMAGE_H
#define JPEGIMAGE_H

#include "IImageContainer.h"
#include "Types.h"

namespace mxp {

class JpegImageContainer : public IImageContainer {
public:
  JpegImageContainer();

  virtual const char* GetExtension() const override;

  virtual const char* GetFourCC() const override;

  virtual uint32_t GetBpp() const override;

  virtual size_t GetSize() const noexcept override;

  virtual bool Compress(ImagePtr image) override;

  virtual bool Save(const mstring& filename) const override;

private:
  ImagePtr m_image;
};

} /* namespace mxp */

#endif /* JPEGIMAGE_H */
