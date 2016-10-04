/*****************************************************************************
* Media Explorer
*****************************************************************************/

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}

#include <vector>
#include <assert.h>
#include "parser/ParserMedia.h"
#include "logging/Logger.h"

void print_error(const char *msg, int err) {
  char errbuf[128];
  const char *errbuf_ptr = errbuf;

  if(av_strerror(err, errbuf, sizeof(errbuf)) < 0)
    errbuf_ptr = strerror(AVUNERROR(err));

  LOG_ERROR(msg, errbuf_ptr);
}

namespace mxp {
namespace parser {

struct Stream {
  Stream(AVCodecContext* codecContext, const AVCodec *codec, const AVMediaType mediaType)
    : CodecContext(codecContext)
    , Codec(codec)
    , MediaType(mediaType) { }

  ~Stream() {
    if (CodecContext != nullptr) {
      avcodec_close(CodecContext);
      CodecContext = nullptr;
    }
  }

  AVCodecContext*       CodecContext;
  const AVCodec*        Codec;
  const AVMediaType     MediaType;
};

struct Metadata {
  Metadata(const std::string&& key, const std::string&& value)
    : Key{ std::move(key) }
    , Value{ std::move(value) }{ }

  const std::string Key;
  const std::string Value;
};

class Context {
public:
  void AddMetadata() {
    AVDictionaryEntry *tag = nullptr;
    int ret;
    while((tag = av_dict_get(m_fmtCtx->metadata, "", tag, AV_DICT_IGNORE_SUFFIX))) {
      LOG_TRACE("Metadata[n]: ",  tag->key, "=", tag->value);
      m_metadata.emplace_back(std::make_shared<Metadata>(std::string(tag->key), std::string(tag->value)));
    }
  }

  void AddStream(AVCodecContext *codecContext, const AVMediaType mediaType) {
    auto codec = avcodec_find_decoder(codecContext->codec_id);
    if(codec == nullptr) {
      LOG_ERROR("Unsupported codec: ", avcodec_get_name(codecContext->codec_id));
    }

    m_streams.emplace_back(std::make_shared<Stream>(codecContext, codec, mediaType));
  }

  void ClearStreams() {
    LOG_TRACE("Clearing streams");
    m_streams.erase(m_streams.begin(), m_streams.end());
  }

  void CloseFormatContext() {
    if(m_fmtCtx != nullptr) {
      LOG_TRACE("Clearing Format Context");
      avformat_close_input(&m_fmtCtx);
      m_fmtCtx = nullptr;
    }
  }

  AVFormatContext* GetFormatContext() const { return m_fmtCtx; }

  void SetFormatContext(AVFormatContext* context) { m_fmtCtx = context; }

public:
  AVFormatContext*                       m_fmtCtx;
  std::vector<std::shared_ptr<Stream>>   m_streams;
  std::vector<std::shared_ptr<Metadata>> m_metadata;
  std::shared_ptr<MediaContainer>        m_mediaContainer;
};
}
}

mxp::parser::ParserMedia::ParserMedia(const std::shared_ptr<mxp::fs::IFile> file)
  : m_file(file)
  , m_context(std::make_unique<Context>()) {
  assert(file != nullptr && file.get() != nullptr);

  auto filename = m_file->FullPath().c_str();
  LOG_DEBUG("Parsing media for file '", filename, "'");

  int err;
  if((err = avformat_open_input(&m_context->m_fmtCtx, filename, nullptr, nullptr)) < 0) {
    print_error("Failed to create media container: ", err);
  } else if((err = avformat_find_stream_info(m_context->m_fmtCtx, nullptr)) < 0) {
    print_error("Cannot find stream information: ", err);
  } else {
    auto fmt = m_context->m_fmtCtx->iformat;
    m_context->m_mediaContainer = std::make_shared<MediaContainer>(fmt->name, fmt->long_name, fmt->extensions);

    LOG_INFO("Media file '", filename, "' contains ", m_context->m_fmtCtx->nb_streams, " streams.");

    for(auto i = 0; i < m_context->m_fmtCtx->nb_streams; i++) {
      auto context = m_context->m_fmtCtx->streams[i]->codec;
      LOG_DEBUG("Stream[", i, "] = { '", filename, "', ", av_get_media_type_string(context->codec_type), ", ", avcodec_get_name(context->codec_id), " }");

      if(context->codec_type == AVMEDIA_TYPE_VIDEO) {
        m_context->AddStream(context, context->codec_type);
      } else if(context->codec_type == AVMEDIA_TYPE_AUDIO) {
        m_context->AddStream(context, context->codec_type);
      } else if(context->codec_type == AVMEDIA_TYPE_SUBTITLE) {
        m_context->AddStream(context, context->codec_type);
      } else {
        LOG_WARN("Unsupported media stream type '", av_get_media_type_string(context->codec_type), "' in '", filename, "'. Ignoring.");
      }
    }

    LOG_INFO("Loading metadata for media file '", filename, "'");
    m_context->AddMetadata();
  }
}

mxp::parser::ParserMedia::~ParserMedia() {
  if(m_context->m_fmtCtx != nullptr) {
    m_context->ClearStreams();
    m_context->CloseFormatContext();
  }
}

std::shared_ptr<mxp::parser::MediaContainer> mxp::parser::ParserMedia::GetMediaContainer() {
  return m_context->m_mediaContainer;
}
