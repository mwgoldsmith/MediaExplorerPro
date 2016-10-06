/*****************************************************************************
* Media Explorer
*****************************************************************************/

#if HAVE_CONFIG_H
# include "config.h"
#endif

extern "C" {
#include <libavutil/avutil.h>
}

#include "av/AvType.h"
#include "logging/Logger.h"

mxp::av::AvType mxp::av::GetAvType(int libAvType) {
  auto extType = static_cast<AVMediaType>(libAvType);

  switch(extType) {
  case AVMEDIA_TYPE_VIDEO:
    return AvType::Video;
  case AVMEDIA_TYPE_AUDIO:
    return AvType::Audio;
  case AVMEDIA_TYPE_SUBTITLE:
    return AvType::Subtitles;
  case AVMEDIA_TYPE_UNKNOWN:
    return AvType::None;
  case AVMEDIA_TYPE_DATA:
  case AVMEDIA_TYPE_ATTACHMENT:
  case AVMEDIA_TYPE_NB:
    LOG_DEBUG("Unhandled media type: ", av_get_media_type_string(extType));
    return AvType::None;
  default:
    break;
  }

  LOG_WARN("Unsupported media type: ", libAvType);

  return AvType::None;
}

std::string mxp::av::GetAvTypeString(AvType type) {
  switch(type) {
  case AvType::Audio:
    return "Audio";
  case AvType::None:
    return "None";
  case AvType::Subtitles:
    return "Subtitles";
  case AvType::Video:
    return "Video";
  case AvType::Image:
    return "Image";
  default:
    break;
  }

  LOG_WARN("Unsupported media type: ", type);

  return{};
}