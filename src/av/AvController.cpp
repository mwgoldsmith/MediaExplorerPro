/*****************************************************************************
* Media Explorer
*****************************************************************************/

#include "stdafx.h"
#if HAVE_CONFIG_H
#  include "config.h"
#endif

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/log.h>
}

#include "Codec.h"
#include "Container.h"
#include "MediaExplorer.h"
#include "av/AvCodec.h"
#include "av/AvContainer.h"
#include "av/AvController.h"
#include "compat/Mutex.h"
#include "logging/Logger.h"

extern "C" {

/**
 * @brief Mutex used to provide thread-safety in the AvLogCallback function
 */
static mxp::compat::Mutex s_avLogLock;

/**
 * @brief

 * @param avcl
 * @param level
 * @param format
 * @param vl
 */
static void AvLogCallback(void* avcl, int level, const char* format, va_list vl) {
  if (level != AV_LOG_ERROR && level != AV_LOG_WARNING && level != AV_LOG_INFO && level != AV_LOG_VERBOSE && level != AV_LOG_DEBUG) {
    return;
  }

  s_avLogLock.lock();

  char buffer[512];
  vsnprintf(buffer, sizeof(buffer) / sizeof(char), format, vl);
  auto msg =std::string( buffer);

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

} /* extern "C" */

using AvContainerVector = std::vector<mxp::av::AvContainer>;
using AvCodecVector = std::vector<mxp::av::AvCodec>;

mxp::MediaExplorerPtr mxp::av::AvController::s_ml = nullptr;
std::unique_ptr< mxp::av::AvController::ContainerVector> mxp::av::AvController::s_containers = {};
std::unique_ptr< mxp::av::AvController::CodecVector> mxp::av::AvController::s_codecs = {};
mxp::av::AvController::ContainerType mxp::av::AvController::s_containerTypes[] = {
  { "avi", MediaType::Video },
  { "cdxl", MediaType::Video },
  { "dv", MediaType::Video },
  { "flv", MediaType::Video },
  { "iv8", MediaType::Video },
  { "live_flv", MediaType::Video },
  { "mlv", MediaType::Video },
  { "matroska,webm", MediaType::Video },
  { "nsv", MediaType::Video },
  { "nuv", MediaType::Video },
  { "mov,mp4,m4a,3gp,3g2,mj2", MediaType::Video },
  { "cavsvideo", MediaType::Video },
  { "h264", MediaType::Video },
  { "hevc", MediaType::Video },
  { "mjpeg", MediaType::Video },
  { "mpegvideo", MediaType::Video },
  { "m4v", MediaType::Video },
  { "rawvideo", MediaType::Video },
  { "aqtitle", MediaType::Subtitles },
  { "jacosub", MediaType::Subtitles },
  { "lrc", MediaType::Subtitles },
  { "microdvd", MediaType::Subtitles },
  { "mpl2", MediaType::Subtitles },
  { "mpsub", MediaType::Subtitles },
  { "pjs", MediaType::Subtitles },
  { "sup", MediaType::Subtitles },
  { "realtext", MediaType::Subtitles },
  { "sami", MediaType::Subtitles },
  { "stl", MediaType::Subtitles },
  { "ass", MediaType::Subtitles },
  { "srt", MediaType::Subtitles },
  { "subviewer", MediaType::Subtitles },
  { "subviewer1", MediaType::Subtitles },
  { "vobsub", MediaType::Subtitles },
  { "vplayer", MediaType::Subtitles },
  { "webvtt", MediaType::Subtitles },
  { "caf", MediaType::Audio },
  { "ast", MediaType::Audio },
  { "aiff", MediaType::Audio },
  { "avr", MediaType::Audio },
  { "boa", MediaType::Audio },
  { "daud", MediaType::Audio },
  { "epaf", MediaType::Audio },
  { "loas", MediaType::Audio },
  { "aea", MediaType::Audio },
  { "ape", MediaType::Audio },
  { "mp3", MediaType::Audio },
  { "aac", MediaType::Audio },
  { "oma", MediaType::Audio },
  { "tta", MediaType::Audio },
  { "wav", MediaType::Audio },
  { "wsaud", MediaType::Audio }
};

mxp::MediaType mxp::av::GetMediaType(int libAvType) {
  auto extType = static_cast<AVMediaType>(libAvType);

  switch(extType) {
  case AVMEDIA_TYPE_VIDEO:
    return MediaType::Video;
  case AVMEDIA_TYPE_AUDIO:
    return MediaType::Audio;
  case AVMEDIA_TYPE_SUBTITLE:
    return MediaType::Subtitles;
  case AVMEDIA_TYPE_UNKNOWN:
    return MediaType::None;
  case AVMEDIA_TYPE_DATA:
  case AVMEDIA_TYPE_ATTACHMENT:
  case AVMEDIA_TYPE_NB:
    LOG_DEBUG("Unhandled media type: ", av_get_media_type_string(extType));
    return MediaType::None;
  default:
    break;
  }

  LOG_WARN("Unsupported media type: ", libAvType);

  return MediaType::None;
}

std::string mxp::av::GetMediaTypeString(MediaType type) {
  switch(type) {
  case MediaType::Audio:
    return "Audio";
  case MediaType::None:
    return "None";
  case MediaType::Subtitles:
    return "Subtitles";
  case MediaType::Video:
    return "Video";
  case MediaType::Image:
    return "Image";
  default:
    break;
  }

  LOG_WARN("Unsupported media type: ", (int)type);

  return{};
}

bool mxp::av::AvController::Initialize(MediaExplorerPtr ml) {
  LOG_DEBUG("Initializing the AvController");

  av_log_set_callback(AvLogCallback);
  av_log_set_level(AV_LOG_WARNING);
  av_register_all();

  s_ml = ml;

 
  std::function<void(const ContainerPtr &)> cbMarkContainer = [](const ContainerPtr& c) { c->SetSupported(false); };
  std::function<void(const AvContainer &)> cbNewContainer = [ml](const AvContainer& c) {
    auto type = MediaType::None;
    for(auto const& t : s_containerTypes) {
      if(t.Name.compare(c.GetName()) == 0) {
        type = t.Type;
        break;
      }
    }

    auto container = Container::Create(ml, c.GetName(), c.GetLongName(), c.GetExtensions(), c.GetMimeType(), type);
    s_containers->push_back(container);
  };


  // Get existing containers
  auto avContainers = GetAvContainers();
  auto containers = ml->ContainerList(SortingCriteria::Default, false);
  s_containers = std::unique_ptr<ContainerVector>(std::make_unique<ContainerVector>(containers));

  // Get changes to supported containers
  GetSupportDiff(containers, avContainers, "Container", cbMarkContainer);
  GetSupportDiff(avContainers, containers, "container", cbNewContainer);


  std::function<void(const CodecPtr &)> cbMarkCodec = [](const CodecPtr& c) { c->SetSupported(false); };
  std::function<void(const AvCodec &)> cbNewCodec = [ml](const AvCodec& c) {
    auto codec = Codec::Create(ml, c.GetName(), c.GetLongName(), c.GetType());
    s_codecs->push_back(codec);
  };

  // Get existing codecs
  auto avCodecs = GetAvCodecs();
  auto codecs = ml->CodecList(SortingCriteria::Default, false);
  s_codecs = std::unique_ptr<CodecVector>(std::make_unique<CodecVector>(codecs));

  // Get changes to supported codecs
  GetSupportDiff(codecs, avCodecs, "Codec", cbMarkCodec);
  GetSupportDiff(avCodecs, codecs, "codec", cbNewCodec);

  return true;
}

std::vector<mxp::av::AvContainer> mxp::av::AvController::GetAvContainers() {
  std::vector<AvContainer> avContainers;

  LOG_DEBUG("Enumerating input formats");

  auto fmt = av_iformat_next(nullptr);
  while(fmt != nullptr) {
    auto longName = fmt->long_name != nullptr ? fmt->long_name : "";
    auto extensions = fmt->extensions != nullptr ? fmt->extensions : "";
    auto mimeType = fmt->mime_type != nullptr ? fmt->mime_type : "";

    LOG_DEBUG("Format: ", fmt->name, "; ", longName, "; ", extensions, "; ", mimeType);
    avContainers.emplace_back(fmt->name, longName, extensions, mimeType);

    fmt = av_iformat_next(fmt);
  }

  return avContainers;
}

std::vector<mxp::av::AvCodec> mxp::av::AvController::GetAvCodecs() {
  std::vector<AvCodec> avCodecs;

  LOG_DEBUG("Enumerating codecs");

  auto codec = av_codec_next(nullptr);
  auto prevType = MediaType::None;
  mstring prevName = "";

  while(codec != nullptr) {
    auto curType = GetMediaType(codec->type);
    auto curName = to_mstring(codec->name);

    if((prevName.compare(curName) != 0) || (prevType != curType)) {
      auto longName = codec->long_name != nullptr ? codec->long_name : "";

      LOG_DEBUG("Codec: ", GetMediaTypeString(curType), "; ", curName, "; ", longName);
      avCodecs.emplace_back(AvCodec(curName.c_str(), longName, curType));
    }

    prevName = curName;
    prevType = curType;

    codec = av_codec_next(codec);
  }

  return avCodecs;
}

mxp::ContainerPtr mxp::av::AvController::FindContainer(const mstring name) {
  ContainerPtr result = nullptr;
  auto containers = s_containers.get();

  for(auto const& t : *containers) {
    if(t->GetName().compare(name) == 0) {
      result = t;
      break;
    }
  }

  return result;
}

mxp::CodecPtr mxp::av::AvController::FindCodec(const mstring name, MediaType type) {
  CodecPtr result = nullptr;
  auto codecs = s_codecs.get();

  for(auto const& t : *codecs) {
    if((t->GetName().compare(name) == 0) && (t->GetType() == type)) {
      result = t;
      break;
    }
  }

  return result;
}
