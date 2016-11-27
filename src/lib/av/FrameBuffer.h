/*****************************************************************************
* Media Explorer
*****************************************************************************/

#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "mediaexplorer/Common.h"

struct AVFrame;

namespace mxp {

class FrameBuffer {
public:
  FrameBuffer();

  ~FrameBuffer();

  AVFrame* GetFrame() const;

  int GetSize() const;

  static std::unique_ptr<FrameBuffer> Create(int width, int height);

private:
  AVFrame* m_frame;
  uint8_t* m_buffer;
  int      m_size;
};

}

#endif /* FRAMEBUFFER_H */
