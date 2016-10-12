/*****************************************************************************
* Media Explorer
*****************************************************************************/

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
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

struct ParserStream {
  ParserStream(AVCodecContext* codecContext, const AVCodec *codec, const AVMediaType mediaType, uint32_t index)
    : CodecContext(codecContext)
    , Codec(codec)
    , AvType(mediaType)
    , Index(index) { }

  ~ParserStream() {
    if (CodecContext != nullptr) {
      avcodec_close(CodecContext);
      CodecContext = nullptr;
    }
  }

  AVCodecContext*       CodecContext;
  const AVCodec*        Codec;
  const AVMediaType     AvType;
  uint32_t              Index;
};

struct ParserMetadata {
  ParserMetadata(const std::string&& key, const std::string&& value, uint32_t streamIndex)
    : Key{ std::move(key) }
    , Value{ std::move(value) }
    , StreamIndex{ streamIndex } {
  }

  const std::string Key;
  const std::string Value;
  const uint32_t    StreamIndex;
};

class Context {
public:
  void AddMetadata(const AVDictionary* dict, uint32_t streamIndex) {
    AVDictionaryEntry *tag = nullptr;

    while((tag = av_dict_get(dict, "", tag, AV_DICT_IGNORE_SUFFIX))) {
      LOG_TRACE("Metadata[", m_metadata.size(), "]: ",  tag->key, "=", tag->value);
      m_metadata.emplace_back(std::make_shared<ParserMetadata>(std::string(tag->key), std::string(tag->value), streamIndex));
    }
  }

  void AddStream(AVCodecContext *codecContext, const AVMediaType mediaType, uint32_t index) {
    auto codec = avcodec_find_decoder(codecContext->codec_id);
    if(codec == nullptr) {
      LOG_ERROR("Unsupported codec: ", avcodec_get_name(codecContext->codec_id));
    }

    m_streams.emplace_back(std::make_shared<ParserStream>(codecContext, codec, mediaType, index));
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

public:
  AVFormatContext*                       m_fmtCtx;
  std::vector<std::shared_ptr<ParserStream>>   m_streams;
  std::vector<std::shared_ptr<ParserMetadata>> m_metadata;
  std::shared_ptr<av::AvContainer>       m_mediaContainer;
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

    m_context->m_mediaContainer = std::make_shared<av::AvContainer>(fmt->name, fmt->long_name, fmt->extensions, fmt->mime_type);

    LOG_INFO("Media file '", filename, "' contains ", m_context->m_fmtCtx->nb_streams, " streams.");

    for(unsigned int i = 0; i < m_context->m_fmtCtx->nb_streams; i++) {
      auto stream = m_context->m_fmtCtx->streams[i];
      
      auto context = stream->codec;
      LOG_DEBUG("Stream[", i, "] = { '", filename, "', ", av_get_media_type_string(context->codec_type), ", ", avcodec_get_name(context->codec_id), " }");

      m_context->AddMetadata(stream->metadata, static_cast<uint32_t>(i));

      if(context->codec_type == AVMEDIA_TYPE_VIDEO) {
        m_context->AddStream(context, context->codec_type, i);
      } else if(context->codec_type == AVMEDIA_TYPE_AUDIO) {
        m_context->AddStream(context, context->codec_type, i);
      } else if(context->codec_type == AVMEDIA_TYPE_SUBTITLE) {
        m_context->AddStream(context, context->codec_type, i);
      } else {
        LOG_WARN("Unsupported media stream type '", av_get_media_type_string(context->codec_type), "' in '", filename, "'. Ignoring.");
      }
    }

    LOG_INFO("Loading metadata for media file '", filename, "'");
    m_context->AddMetadata(m_context->m_fmtCtx->metadata, static_cast<uint32_t>(-1));
  }
}

mxp::parser::ParserMedia::~ParserMedia() {
  if(m_context->m_fmtCtx != nullptr) {
    m_context->ClearStreams();
    m_context->CloseFormatContext();
  }
}

std::shared_ptr<mxp::av::AvContainer> mxp::parser::ParserMedia::GetMediaContainer() {
  return m_context->m_mediaContainer;
}
