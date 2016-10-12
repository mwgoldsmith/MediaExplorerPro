
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
#include "Metadata.h"
#include "Media.h"
#include "Stream.h"
#include "logging/Logger.h"
#include "parser/ParserMedia.h"
#include <av/AvController.h>

void print_error(const char *msg, int err) {
  char errbuf[128];
  const char *errbuf_ptr = errbuf;

  if(av_strerror(err, errbuf, sizeof(errbuf)) < 0)
    errbuf_ptr = strerror(AVUNERROR(err));

  LOG_ERROR(msg, errbuf_ptr);
}

namespace mxp {
namespace parser {

//struct ParserStream {
//  ParserStream(AVCodecContext* codecContext, const AVCodec *codec, const AVMediaType mediaType, uint32_t index)
//    : CodecContext(codecContext)
//    , Codec(codec)
//    , AvType(mediaType)
//    , Index(index) { }
//
//  ~ParserStream() {
//    if (CodecContext != nullptr) {
//      avcodec_close(CodecContext);
//      CodecContext = nullptr;
//    }
//  }
//
//  AVCodecContext*       CodecContext;
//  const AVCodec*        Codec;
//  const AVMediaType     AvType;
//  uint32_t              Index;
//};
//
//struct ParserMetadata {
//  ParserMetadata(const std::string&& key, const std::string&& value, uint32_t streamIndex)
//    : Key{ std::move(key) }
//    , Value{ std::move(value) }
//    , StreamIndex{ streamIndex } {
//  }
//
//  const std::string Key;
//  const std::string Value;
//  const uint32_t    StreamIndex;
//};

class Context {
public:
  //void AddMetadata(MediaExplorerPtr ml, const AVDictionary* dict, uint32_t streamIndex) {
  //  AVDictionaryEntry *tag = nullptr;

  //  while((tag = av_dict_get(dict, "", tag, AV_DICT_IGNORE_SUFFIX))) {
  //    LOG_TRACE("Metadata[", m_metadata.size(), "]: ",  tag->key, "=", tag->value);
  //    m_metadata.emplace_back(std::make_shared<ParserMetadata>(std::string(tag->key), std::string(tag->value), streamIndex));

  //    mxp::Metadata::Create(ml, tag->key, tag->value);
  //  }
  //}

  //void AddStream(AVCodecContext *codecContext, const AVMediaType mediaType, uint32_t index) {
  //  auto codec = avcodec_find_decoder(codecContext->codec_id);
  //  if(codec == nullptr) {
  //    LOG_ERROR("Unsupported codec: ", avcodec_get_name(codecContext->codec_id));
  //  }

  //  m_streams.emplace_back(std::make_shared<ParserStream>(codecContext, codec, mediaType, index));
  //}

  //void ClearStreams() {
  //  LOG_TRACE("Clearing streams");
  //  m_streams.erase(m_streams.begin(), m_streams.end());
  //}

  //void CloseFormatContext() {
  //  if(m_fmtCtx != nullptr) {
  //    LOG_TRACE("Clearing Format Context");
  //    avformat_close_input(&m_fmtCtx);
  //    m_fmtCtx = nullptr;
  //  }
  //}

public:
  AVFormatContext*                       m_fmtCtx;
 // std::vector<std::shared_ptr<ParserStream>>   m_streams;
 // std::vector<std::shared_ptr<ParserMetadata>> m_metadata;
  ContainerPtr       m_mediaContainer;
};
}
}

mxp::parser::ParserMedia::ParserMedia(MediaExplorerPtr ml, std::shared_ptr<Media> media, const std::shared_ptr<mxp::fs::IFile> file)
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
    m_context->m_mediaContainer = av::AvController::FindContainer(fmt->name);

    LOG_INFO("Media file '", filename, "' contains ", m_context->m_fmtCtx->nb_streams, " streams.");

    AVDictionaryEntry *tag = nullptr;
    while((tag = av_dict_get(m_context->m_fmtCtx->metadata, "", tag, AV_DICT_IGNORE_SUFFIX))) {
      LOG_TRACE("Metadata: ", tag->key, "=", tag->value);

      auto metadata = mxp::Metadata::Create(ml, tag->key, tag->value);
      media->AddMetadata(metadata->Id());
    }

    for(unsigned int i = 0; i < m_context->m_fmtCtx->nb_streams; i++) {
      auto context = m_context->m_fmtCtx->streams[i]->codec;
      if(!(context->codec_type == AVMEDIA_TYPE_VIDEO) || (context->codec_type == AVMEDIA_TYPE_AUDIO) || (context->codec_type == AVMEDIA_TYPE_SUBTITLE)) {
        LOG_WARN("Unsupported media stream type '", av_get_media_type_string(context->codec_type), "' in '", filename, "'. Ignoring.");
        continue;
      }

      auto decoder = avcodec_find_decoder(context->codec_id);
      if(decoder == nullptr) {
        LOG_ERROR("Unsupported codec: ", avcodec_get_name(context->codec_id));
        continue;
      }

      LOG_DEBUG("Stream[", i, "] = { '", filename, "', ", av_get_media_type_string(context->codec_type), ", ", avcodec_get_name(context->codec_id), " }");

      auto type = mxp::av::GetMediaType(decoder->type);
      auto codec = av::AvController::FindCodec(decoder->name, type);
      auto stream = mxp::Stream::Create(ml, codec, i, media, type);

      while((tag = av_dict_get(m_context->m_fmtCtx->streams[i]->metadata, "", tag, AV_DICT_IGNORE_SUFFIX))) {
        LOG_TRACE("Metadata: ", tag->key, "=", tag->value);

        auto metadata = mxp::Metadata::Create(ml, tag->key, tag->value);
        stream->AddMetadata(metadata->Id());
      }

      avcodec_close(context);
    }

  }
}

mxp::parser::ParserMedia::~ParserMedia() {
  if(m_context->m_fmtCtx != nullptr) {
    //m_context->ClearStreams();
    avformat_close_input(&m_context->m_fmtCtx);
   // m_context->CloseFormatContext();
  }
}

mxp::ContainerPtr mxp::parser::ParserMedia::GetMediaContainer() {
  return m_context->m_mediaContainer;
}
