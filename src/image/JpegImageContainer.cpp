/*****************************************************************************
* Media Explorer
*****************************************************************************/

#include "stdafx.h"
#if HAVE_CONFIG_H
# include "config.h"
#endif

#include <jpeglib.h>
#include <setjmp.h>
#include "JpegImageContainer.h"
#include "Types.h"
#include "logging/Logger.h"
#include "image/Image.h"

struct jpegError : public jpeg_error_mgr {
  jmp_buf buff;
  char message[JMSG_LENGTH_MAX];

  static void jpegErrorHandler(j_common_ptr common) {
    auto error = reinterpret_cast<jpegError*>(common->err);
    (*error->format_message)(common, error->message);
    longjmp(error->buff, 1);
  }
};

mxp::JpegImageContainer::JpegImageContainer() {}

const char* mxp::JpegImageContainer::GetExtension() const {
  return "jpg";
}

const char* mxp::JpegImageContainer::GetFourCC() const {
  return "RV24";
}

uint32_t mxp::JpegImageContainer::GetBpp() const {
  return 3;
}

size_t mxp::JpegImageContainer::GetSize() const noexcept {
  return m_image == nullptr ? 0 : m_image->GetSize();
}

bool mxp::JpegImageContainer::Compress(ImagePtr image) {
  unsigned char *dst = nullptr;
  unsigned long dstSize = 0;
  auto buffer = image->GetData();
  auto width = image->GetX();
  auto height = image->GetY();
  const auto vOffset = 0;
  const auto hOffset = 0;
  const auto stride = width * GetBpp();

  jpeg_compress_struct cinfo;
  JSAMPROW row_pointer[1];

  jpegError err;
  cinfo.err = jpeg_std_error(&err);
  err.error_exit = jpegError::jpegErrorHandler;

  if(setjmp(err.buff)) {
    LOG_ERROR("JPEG failure: ", err.message);
    jpeg_destroy_compress(&cinfo);
    return false;
  }

  jpeg_create_compress(&cinfo);
  jpeg_mem_dest(&cinfo, &dst, &dstSize);
  cinfo.image_width = width;
  cinfo.image_height = height;
  cinfo.input_components = GetBpp();
  cinfo.in_color_space = JCS_RGB;
  jpeg_set_defaults(&cinfo);
  jpeg_set_quality(&cinfo, 85, TRUE);

  jpeg_start_compress(&cinfo, TRUE);
  while(cinfo.next_scanline < height) {
    row_pointer[0] = const_cast<uint8_t*>(&buffer[(cinfo.next_scanline + vOffset) * stride + hOffset * GetBpp()]);
    jpeg_write_scanlines(&cinfo, row_pointer, 1);
  }

  jpeg_finish_compress(&cinfo);
  jpeg_destroy_compress(&cinfo);

  m_image = std::make_shared<Image>(width, height, GetBpp());
  if(m_image->Assign(dst, dstSize) == false) {
    m_image = {};
    return false;
  }

  return true;
}

bool mxp::JpegImageContainer::Save(const mstring& filename) const {
  if(m_image == nullptr || m_image->GetSize() <= 0) {
    return false;
  }

  auto pFile = fopen(filename.c_str(), "wb");
  if(pFile == nullptr) {
    return false;
  }

  auto success = true;
  auto count = m_image->GetSize();
  if(fwrite(m_image->GetData(), sizeof(uint8_t), count, pFile) != count) {
    success = false;
  }

  fclose(pFile);

  return success;
}
