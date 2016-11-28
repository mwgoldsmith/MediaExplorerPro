/*****************************************************************************
* Media Explorer
*****************************************************************************/

#include "stdafx.h"
#if HAVE_CONFIG_H
# include "config.h"
#endif

#include "av/MediaContext.h"
#include "av/StreamType.h"
#include "logging/Logger.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
}

static void print_error(const char *msg, int err) {
  char errbuf[128];
  const char *errbuf_ptr = errbuf;

  if(av_strerror(err, errbuf, sizeof(errbuf)) < 0)
    errbuf_ptr = strerror(AVUNERROR(err));

  LOG_ERROR(msg, errbuf_ptr);
}

mxp::MediaContext::MediaContext(const mstring& filename)
  : m_filename{filename}
  , m_formatCtx{nullptr}
  , m_stream{nullptr}
  , m_codec{nullptr}
  , m_codecCtx{nullptr}
  , m_pts {0}
  , m_duration(0)
  , m_index{-1} {}

mxp::MediaContext::~MediaContext() {
  Close();
}

bool mxp::MediaContext::Open() {
  int err;

  if((err = avformat_open_input(&m_formatCtx, m_filename.c_str(), nullptr, nullptr)) < 0) {
    print_error("Failed to open input media: ", err);
  } else if((err = avformat_find_stream_info(m_formatCtx, nullptr)) < 0) {
    print_error("Failed to find stream info: ", err);
  } else {
    auto duration = m_formatCtx->duration + 5000;
    m_duration = static_cast<double>(duration) / AV_TIME_BASE;    
  }

  return err >= 0;
}


std::vector<AVDictionaryEntry*> GetDictionaryEntries(AVDictionary* dict) {
  std::vector<AVDictionaryEntry*> result;

  AVDictionaryEntry *tag = nullptr;
  while((tag = av_dict_get(dict, "", tag, AV_DICT_IGNORE_SUFFIX))) {
    result.push_back(tag);
  }

  return result;
}

std::vector<AVDictionaryEntry*> mxp::MediaContext::GetMetadata() const {
  return GetDictionaryEntries(m_formatCtx->metadata);
}

std::vector<AVDictionaryEntry*> mxp::MediaContext::GetStreamMetadata() const {
  return GetDictionaryEntries(m_formatCtx->streams[m_index]->metadata);
}

void mxp::MediaContext::Close() {
  CloseStream();

  if(m_formatCtx != nullptr) {
    avformat_close_input(&m_formatCtx);
    m_formatCtx = nullptr;
  }

  m_duration = 0;
}

static bool IsStreamType(StreamType type, AVMediaType n) {
  auto result = false;
  if(type == StreamType::Video) {
    result = n == AVMEDIA_TYPE_VIDEO;
  } else if(type == StreamType::Audio) {
    result = n == AVMEDIA_TYPE_AUDIO;
  } else if(type == StreamType::Data) {
    result = n == AVMEDIA_TYPE_DATA;
  } else if(type == StreamType::Unknown) {
    result = n == AVMEDIA_TYPE_UNKNOWN;
  } else if(type == StreamType::Subtitle) {
    result = n == AVMEDIA_TYPE_SUBTITLE;
  } else if(type == StreamType::NB) {
    result = n == AVMEDIA_TYPE_NB;
  }

  return result;
}

bool mxp::MediaContext::OpenStream(StreamType type, int index) {
  if(m_formatCtx == nullptr) {
    return false;
  }

  // Close current open stream if there is one
  CloseStream();

  // Find index of stream
  auto count = 0;
  for(unsigned int i = 0; i < m_formatCtx->nb_streams; i++) {
    auto codec = m_formatCtx->streams[i]->codec;
    if(IsStreamType(type, codec->codec_type)) {
      if(count != index) {
        count++;
      } else {
        m_index = i;
        break;
      }
    }
  }

  if(m_index == -1) {
    return false;
  }

  m_stream = m_formatCtx->streams[m_index];
  m_codec = avcodec_find_decoder(m_stream->codec->codec_id);
  if(m_codec == nullptr) {
    LOG_ERROR("Unsupported codec: ", avcodec_get_name(m_stream->codec->codec_id));
  } else {
    m_codecCtx = avcodec_alloc_context3(m_codec);

    // Copy the codec context and open the codec
    if(m_codecCtx == nullptr) {
      LOG_ERROR("Failed to allocate codec context");
      m_codec = nullptr;
    } else {
      int err;
      if((err = avcodec_copy_context(m_codecCtx, m_stream->codec)) < 0) {
        print_error("Failed to copy codec context: ", err);
        m_codec = nullptr;
      } else if(avcodec_open2(m_codecCtx, m_codec, nullptr) < 0) {
        LOG_ERROR("Failed to open codec");
        m_codec = nullptr;
      }

      // If a failure occurred, free the allocated context
      if(m_codec == nullptr) {
        avcodec_free_context(&m_codecCtx);
        m_codecCtx = nullptr;
      }
    } 
  }

  if(m_codec == nullptr) {
    m_stream = nullptr;
    m_index = -1;
    return false;
  }

  return true;
}

bool mxp::MediaContext::CloseStream() {
  if(m_codecCtx != nullptr) {
    avcodec_close(m_codecCtx);
    avcodec_free_context(&m_codecCtx);
  }

  m_stream = nullptr;
  m_codecCtx = nullptr;
  m_codec = nullptr;
  m_index = -1;

  return true;
}

int mxp::MediaContext::GetStreamCount(StreamType type) const {
  if(m_formatCtx == nullptr) {
    return -1;
  }

  auto count = 0;
  for(unsigned int i = 0; i < m_formatCtx->nb_streams; i++) {
    auto codec = m_formatCtx->streams[i]->codec;
    if(IsStreamType(type, codec->codec_type)) {
      count++;
    }
  }

  return count;
}
