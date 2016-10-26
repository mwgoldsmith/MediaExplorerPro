/*****************************************************************************
* Media Explorer
*****************************************************************************/

#include "stdafx.h"
#if HAVE_CONFIG_H
# include "config.h"
#endif

#include "av/FrameBuffer.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
}

mxp::FrameBuffer::FrameBuffer()
  : m_frame{ nullptr }
  , m_buffer{ nullptr } {
}

mxp::FrameBuffer::~FrameBuffer() {
  if(m_frame != nullptr)
    av_free(m_frame);
  if(m_buffer != nullptr)
    av_free(m_buffer);
}

AVFrame* mxp::FrameBuffer::GetFrame() const {
  return m_frame;
}

int mxp::FrameBuffer::GetSize() const {
  return m_size;
}

std::unique_ptr<mxp::FrameBuffer> mxp::FrameBuffer::Create(int width, int height) {
  auto self = std::make_unique<FrameBuffer>();
  self->m_frame = av_frame_alloc();
  if(self->m_frame == nullptr)
    return nullptr;

  self->m_size = av_image_get_buffer_size(PIX_FMT_RGB24, width, height, 1);
  self->m_buffer = static_cast<uint8_t *>(av_malloc(self->m_size * sizeof(uint8_t)));
  if(self->m_buffer == nullptr) {
    return nullptr;
  }

  self->m_frame->format = AV_PIX_FMT_RGB24;
  self->m_frame->width = width;
  self->m_frame->height = height;
  av_image_fill_arrays(self->m_frame->data, self->m_frame->linesize, self->m_buffer, PIX_FMT_RGB24, width, height, 1);

  return self;
}