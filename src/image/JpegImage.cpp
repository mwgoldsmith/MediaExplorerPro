/*****************************************************************************
* Media Explorer
*****************************************************************************/

#include "stdafx.h"
#if HAVE_CONFIG_H
# include "config.h"
#endif

#include <jpeglib.h>
#include <setjmp.h>
#include "JpegImage.h"
#include "logging/Logger.h"
#include "image/Image.h"
#include "Types.h"

namespace mxp {

const char* JpegImage::GetExtension() const {
  return "jpg";
}

const char* JpegImage::GetFourCC() const {
  return "RV24";
}

uint32_t JpegImage::bpp() const {
  return 3;
}

struct jpegError : public jpeg_error_mgr {
  jmp_buf buff;
  char message[JMSG_LENGTH_MAX];

  static void jpegErrorHandler(j_common_ptr common) {
    auto error = reinterpret_cast<jpegError*>(common->err);
    (*error->format_message)(common, error->message);
    longjmp(error->buff, 1);
  }
};

bool JpegImage::save(ImagePtr image, const std::string& filename) {
  return compress(image->GetData(), filename, image->GetX(), image->GetY(), image->GetX(), image->GetY(), 0, 0);
}

bool JpegImage::compress(const uint8_t* buffer, const std::string& outputFile,
  uint32_t inputWidth, uint32_t,
  uint32_t outputWidth, uint32_t outputHeight,
  uint32_t hOffset, uint32_t vOffset) {
  const auto stride = inputWidth * bpp();

  //FIXME: Abstract this away, though libjpeg requires a FILE*...
  auto fOut = std::unique_ptr<FILE, int(*)(FILE*)>(fopen(outputFile.c_str(), "wb"), &fclose);
  if(fOut == nullptr) {
    LOG_ERROR("Failed to open thumbnail file ", outputFile, '(', strerror(errno), ')');
    return false;
  }

  jpeg_compress_struct compInfo;
  JSAMPROW row_pointer[1];

  //libjpeg's default error handling is to call exit(), which would
  //be slightly problematic...
  jpegError err;
  compInfo.err = jpeg_std_error(&err);
  err.error_exit = jpegError::jpegErrorHandler;

  if(setjmp(err.buff)) {
    LOG_ERROR("JPEG failure: ", err.message);
    jpeg_destroy_compress(&compInfo);
    return false;
  }

  jpeg_create_compress(&compInfo);
  jpeg_stdio_dest(&compInfo, fOut.get());

  compInfo.image_width = outputWidth;
  compInfo.image_height = outputHeight;
  compInfo.input_components = bpp();
  compInfo.in_color_space = JCS_RGB;
  jpeg_set_defaults(&compInfo);
  jpeg_set_quality(&compInfo, 85, TRUE);

  jpeg_start_compress(&compInfo, TRUE);

  while(compInfo.next_scanline < outputHeight) {
    row_pointer[0] = const_cast<uint8_t*>(&buffer[(compInfo.next_scanline + vOffset) * stride + hOffset * bpp()]);
    jpeg_write_scanlines(&compInfo, row_pointer, 1);
  }
  jpeg_finish_compress(&compInfo);
  jpeg_destroy_compress(&compInfo);
  return true;
}

}
