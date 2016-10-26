/*****************************************************************************
* Media Explorer
*****************************************************************************/

#include "stdafx.h"
#if HAVE_CONFIG_H
# include "config.h"
#endif

#include "av/MediaContext.h"
#include "av/StreamType.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}


mxp::MediaContext::MediaContext(const mstring& filename)
  : m_filename{ filename }
  , m_formatCtx{ nullptr }
  , m_stream{ nullptr }
  , m_codec{ nullptr }
  , m_codecCtx{ nullptr }
  , m_pts { 0 }
  , m_index{ -1 } {
}

mxp::MediaContext::~MediaContext() {
  Close();
}

bool mxp::MediaContext::Open() {
  if(avformat_open_input(&m_formatCtx, m_filename.c_str(), nullptr, nullptr) != 0)
    return false;
  if(avformat_find_stream_info(m_formatCtx, nullptr) < 0)
    return false;

  return true;
}

void mxp::MediaContext::Close() {
  CloseStream();

  if(m_formatCtx != nullptr) {
    avformat_close_input(&m_formatCtx);
    m_formatCtx = nullptr;
  }
}

static bool IsStreamType(StreamType type, AVMediaType n) {
  if(type == StreamType::Video) {
    return n == AVMEDIA_TYPE_VIDEO;
  } else if(type == StreamType::Audio) {
    return n == AVMEDIA_TYPE_AUDIO;
  } else if(type == StreamType::Data) {
    return n == AVMEDIA_TYPE_DATA;
  } else if(type == StreamType::Unknown) {
    return n == AVMEDIA_TYPE_UNKNOWN;
  } else if(type == StreamType::Subtitle) {
    return n == AVMEDIA_TYPE_SUBTITLE;
  } else if(type == StreamType::NB) {
    return n == AVMEDIA_TYPE_NB;
  }

  return false;
}

bool mxp::MediaContext::OpenStream(StreamType type, int index) {
  if(m_formatCtx == nullptr) {
    return false;
  }

  // Close current open stream if there is one
  CloseStream();

  // Find index of stream
  auto count = 0;
  for(auto i = 0; i < m_formatCtx->nb_streams; i++) {
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
  if(m_codec != nullptr) {
    m_codecCtx = avcodec_alloc_context3(m_codec);

    // Copy the codec context and open the codec
    if(m_codecCtx != nullptr) {
      if(avcodec_copy_context(m_codecCtx, m_stream->codec) < 0) {
        m_codec = nullptr;
      } else if(avcodec_open2(m_codecCtx, m_codec, nullptr) < 0) {
        m_codec = nullptr;
      }

      // If a failure occurred, free the allocated context
      if(m_codec == nullptr) {
        avcodec_free_context(&m_codecCtx);
        m_codecCtx = nullptr;
      }
    } else {
      m_codec = nullptr;
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

int mxp::MediaContext::GetStreamCount(StreamType type) {
  if(m_formatCtx == nullptr) {
    return -1;
  }

  auto count = 0;

  for(auto i = 0; i < m_formatCtx->nb_streams; i++) {
    auto codec = m_formatCtx->streams[i]->codec;
    if(IsStreamType(type, codec->codec_type)) {
      count++;
    }
  }

  return count;
}
