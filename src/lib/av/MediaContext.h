#pragma once

#include "mediaexplorer/Common.h"
#include "Types.h"
#include "StreamType.h"

struct AVStream;
struct AVCodec;
struct AVFormatContext;
struct AVCodecContext;
struct AVDictionaryEntry;

namespace mxp {
class MediaContext {
public:
  explicit MediaContext(const mstring& filename);
  ~MediaContext();

  bool Open();

  bool OpenStream(StreamType type, int index);

  bool CloseStream();

  int GetStreamCount(StreamType type) const;

  unsigned int GetIndex() const {
    return m_index;
  }

  AVStream* GetStream() const {
    return m_stream;
  }

  AVCodec* GetCodec() const {
    return m_codec;
  }

  AVFormatContext* GetFormatContext() const {
    return m_formatCtx;
  }

  AVCodecContext* GetCodecContext() const {
    return m_codecCtx;
  }

  std::vector<AVDictionaryEntry*> GetMetadata() const;

  std::vector<AVDictionaryEntry*> GetStreamMetadata() const;

  void SetTimestamp(int64_t value) {
    m_pts = value;
  }

  int64_t GetTimestamp() const {
    return m_pts;
  }

private:
  void Close();

  mstring          m_filename;
  AVFormatContext* m_formatCtx;
  AVStream*        m_stream;
  AVCodec*         m_codec;
  AVCodecContext*  m_codecCtx;
  int64_t          m_pts;
  int              m_index;
};
}
