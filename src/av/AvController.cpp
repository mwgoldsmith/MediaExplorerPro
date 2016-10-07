/*****************************************************************************
* Media Explorer
*****************************************************************************/

#if HAVE_CONFIG_H
# include "config.h"
#endif

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/log.h>
}

#include "av/AvCodec.h"
#include "av/AvContainer.h"
#include "av/AvController.h"
#include "compat/Mutex.h"
#include "logging/Logger.h"

extern "C" {
static mxp::compat::Mutex s_avLogLock;

static void AvLogCallback(void* avcl, int level, const char* format, va_list vl) {
  if (level != AV_LOG_ERROR && level != AV_LOG_WARNING && level != AV_LOG_INFO && level != AV_LOG_VERBOSE && level != AV_LOG_DEBUG) {
    return;
  }

  s_avLogLock.lock();

  char buffer[512];
  vsnprintf(buffer, sizeof(buffer) / sizeof(char), format, vl);
  auto msg = std::string(buffer);

  if(level  == AV_LOG_ERROR) {
    LOG_ERROR(msg);
  } else if(level  == AV_LOG_WARNING) {
    LOG_WARN(msg);
  } else if(level  == AV_LOG_INFO) {
    LOG_INFO(msg);
  } else if(level == AV_LOG_DEBUG) {
    LOG_DEBUG(msg);
  } else if(level == AV_LOG_VERBOSE) {
    LOG_TRACE(msg);
  }

  s_avLogLock.unlock();
}
}


using ContainerVector = std::vector<mxp::av::AvContainer>;
using CodecVector = std::vector<mxp::av::AvCodec>;
std::unique_ptr<ContainerVector> mxp::av::AvController::s_containers = std::unique_ptr<ContainerVector>(std::make_unique<ContainerVector>());
std::unique_ptr<CodecVector> mxp::av::AvController::s_codecs = std::unique_ptr<CodecVector>(std::make_unique<CodecVector>());

bool mxp::av::AvController::Initialize() {
  LOG_DEBUG("Initializing the AvController");
  av_log_set_callback(AvLogCallback);
  av_log_set_level(AV_LOG_WARNING);
  av_register_all();

  LOG_DEBUG("Enumerating input formats");
  auto fmt = av_iformat_next(nullptr);
  while (fmt != nullptr) {
    auto longName = fmt->long_name != nullptr ? fmt->long_name : "";
    auto extensions = fmt->extensions != nullptr ? fmt->extensions : "";
    auto mimeType = fmt->mime_type != nullptr ? fmt->mime_type : "";

    LOG_DEBUG("Format: ", fmt->name, "; ", longName, "; ", extensions, "; ", mimeType);
    s_containers->emplace_back(AvContainer(fmt->name, longName, extensions, mimeType));

    fmt = av_iformat_next(fmt);
  }

  LOG_DEBUG("Enumerating codecs");
  auto codec = av_codec_next(nullptr);
  while(codec != nullptr) {
    auto longName = codec->long_name != nullptr ? codec->long_name : "";
    auto type = GetAvType(codec->type);

    LOG_DEBUG("Codec: ", GetAvTypeString(type), "; ", codec->name, "; ", codec->long_name);
    s_codecs->emplace_back(AvCodec(codec->name, longName, type));

    codec = av_codec_next(codec);
  }

  return true;
}