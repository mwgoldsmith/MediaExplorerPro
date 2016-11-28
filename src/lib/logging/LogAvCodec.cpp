/*****************************************************************************
* Media Explorer
*****************************************************************************/

#include "stdafx.h"
#if HAVE_CONFIG_H
# include "config.h"
#endif
#include "logging/LogAvCodec.h"
#include "logging/Logger.h"


#define LOGENUMVALUE(value) case value: os << #value; break

std::ostream& operator<<(std::ostream& os, const AVRational& obj) {
  os << obj.num << "/" << obj.den;
  return os;
}

std::ostream& operator<<(std::ostream& os, const AVPixelFormat& obj) {
  switch (obj) {
    LOGENUMVALUE(AV_PIX_FMT_NONE);
    LOGENUMVALUE(AV_PIX_FMT_YUV420P);
    LOGENUMVALUE(AV_PIX_FMT_YUYV422);
    LOGENUMVALUE(AV_PIX_FMT_RGB24);
    LOGENUMVALUE(AV_PIX_FMT_BGR24);
    LOGENUMVALUE(AV_PIX_FMT_YUV422P);
    LOGENUMVALUE(AV_PIX_FMT_YUV444P);
    LOGENUMVALUE(AV_PIX_FMT_YUV410P);
    LOGENUMVALUE(AV_PIX_FMT_YUV411P);
    LOGENUMVALUE(AV_PIX_FMT_GRAY8);
    LOGENUMVALUE(AV_PIX_FMT_MONOWHITE);
    LOGENUMVALUE(AV_PIX_FMT_MONOBLACK);
    LOGENUMVALUE(AV_PIX_FMT_PAL8);
    LOGENUMVALUE(AV_PIX_FMT_YUVJ420P);
    LOGENUMVALUE(AV_PIX_FMT_YUVJ422P);
    LOGENUMVALUE(AV_PIX_FMT_YUVJ444P);
    LOGENUMVALUE(AV_PIX_FMT_XVMC_MPEG2_MC);
    LOGENUMVALUE(AV_PIX_FMT_XVMC_MPEG2_IDCT);
    LOGENUMVALUE(AV_PIX_FMT_UYVY422);
    LOGENUMVALUE(AV_PIX_FMT_UYYVYY411);
    LOGENUMVALUE(AV_PIX_FMT_BGR8);
    LOGENUMVALUE(AV_PIX_FMT_BGR4);
    LOGENUMVALUE(AV_PIX_FMT_BGR4_BYTE);
    LOGENUMVALUE(AV_PIX_FMT_RGB8);
    LOGENUMVALUE(AV_PIX_FMT_RGB4);
    LOGENUMVALUE(AV_PIX_FMT_RGB4_BYTE);
    LOGENUMVALUE(AV_PIX_FMT_NV12);
    LOGENUMVALUE(AV_PIX_FMT_NV21);
    LOGENUMVALUE(AV_PIX_FMT_ARGB);
    LOGENUMVALUE(AV_PIX_FMT_RGBA);
    LOGENUMVALUE(AV_PIX_FMT_ABGR);
    LOGENUMVALUE(AV_PIX_FMT_BGRA);
    LOGENUMVALUE(AV_PIX_FMT_GRAY16BE);
    LOGENUMVALUE(AV_PIX_FMT_GRAY16LE);
    LOGENUMVALUE(AV_PIX_FMT_YUV440P);
    LOGENUMVALUE(AV_PIX_FMT_YUVJ440P);
    LOGENUMVALUE(AV_PIX_FMT_YUVA420P);
    LOGENUMVALUE(AV_PIX_FMT_VDPAU_H264);
    LOGENUMVALUE(AV_PIX_FMT_VDPAU_MPEG1);
    LOGENUMVALUE(AV_PIX_FMT_VDPAU_MPEG2);
    LOGENUMVALUE(AV_PIX_FMT_VDPAU_WMV3);
    LOGENUMVALUE(AV_PIX_FMT_VDPAU_VC1);
    LOGENUMVALUE(AV_PIX_FMT_RGB48BE);
    LOGENUMVALUE(AV_PIX_FMT_RGB48LE);
    LOGENUMVALUE(AV_PIX_FMT_RGB565BE);
    LOGENUMVALUE(AV_PIX_FMT_RGB565LE);
    LOGENUMVALUE(AV_PIX_FMT_RGB555BE);
    LOGENUMVALUE(AV_PIX_FMT_RGB555LE);
    LOGENUMVALUE(AV_PIX_FMT_BGR565BE);
    LOGENUMVALUE(AV_PIX_FMT_BGR565LE);
    LOGENUMVALUE(AV_PIX_FMT_BGR555BE);
    LOGENUMVALUE(AV_PIX_FMT_BGR555LE);
    LOGENUMVALUE(AV_PIX_FMT_VAAPI_MOCO);
    LOGENUMVALUE(AV_PIX_FMT_VAAPI_IDCT);
    LOGENUMVALUE(AV_PIX_FMT_VAAPI_VLD);
    LOGENUMVALUE(AV_PIX_FMT_YUV420P16LE);
    LOGENUMVALUE(AV_PIX_FMT_YUV420P16BE);
    LOGENUMVALUE(AV_PIX_FMT_YUV422P16LE);
    LOGENUMVALUE(AV_PIX_FMT_YUV422P16BE);
    LOGENUMVALUE(AV_PIX_FMT_YUV444P16LE);
    LOGENUMVALUE(AV_PIX_FMT_YUV444P16BE);
    LOGENUMVALUE(AV_PIX_FMT_VDPAU_MPEG4);
    LOGENUMVALUE(AV_PIX_FMT_DXVA2_VLD);
    LOGENUMVALUE(AV_PIX_FMT_RGB444LE);
    LOGENUMVALUE(AV_PIX_FMT_RGB444BE);
    LOGENUMVALUE(AV_PIX_FMT_BGR444LE);
    LOGENUMVALUE(AV_PIX_FMT_BGR444BE);
    LOGENUMVALUE(AV_PIX_FMT_GRAY8A);
    LOGENUMVALUE(AV_PIX_FMT_BGR48BE);
    LOGENUMVALUE(AV_PIX_FMT_BGR48LE);
    LOGENUMVALUE(AV_PIX_FMT_YUV420P9BE);
    LOGENUMVALUE(AV_PIX_FMT_YUV420P9LE);
    LOGENUMVALUE(AV_PIX_FMT_YUV420P10BE);
    LOGENUMVALUE(AV_PIX_FMT_YUV420P10LE);
    LOGENUMVALUE(AV_PIX_FMT_YUV422P10BE);
    LOGENUMVALUE(AV_PIX_FMT_YUV422P10LE);
    LOGENUMVALUE(AV_PIX_FMT_YUV444P9BE);
    LOGENUMVALUE(AV_PIX_FMT_YUV444P9LE);
    LOGENUMVALUE(AV_PIX_FMT_YUV444P10BE);
    LOGENUMVALUE(AV_PIX_FMT_YUV444P10LE);
    LOGENUMVALUE(AV_PIX_FMT_YUV422P9BE);
    LOGENUMVALUE(AV_PIX_FMT_YUV422P9LE);
    LOGENUMVALUE(AV_PIX_FMT_VDA_VLD);
    LOGENUMVALUE(AV_PIX_FMT_RGBA64BE);
    LOGENUMVALUE(AV_PIX_FMT_RGBA64LE);
    LOGENUMVALUE(AV_PIX_FMT_BGRA64BE);
    LOGENUMVALUE(AV_PIX_FMT_BGRA64LE);
    LOGENUMVALUE(AV_PIX_FMT_GBRP);
    LOGENUMVALUE(AV_PIX_FMT_GBRP9BE);
    LOGENUMVALUE(AV_PIX_FMT_GBRP9LE);
    LOGENUMVALUE(AV_PIX_FMT_GBRP10BE);
    LOGENUMVALUE(AV_PIX_FMT_GBRP10LE);
    LOGENUMVALUE(AV_PIX_FMT_GBRP16BE);
    LOGENUMVALUE(AV_PIX_FMT_GBRP16LE);
    LOGENUMVALUE(AV_PIX_FMT_YUVA420P9BE);
    LOGENUMVALUE(AV_PIX_FMT_YUVA420P9LE);
    LOGENUMVALUE(AV_PIX_FMT_YUVA422P9BE);
    LOGENUMVALUE(AV_PIX_FMT_YUVA422P9LE);
    LOGENUMVALUE(AV_PIX_FMT_YUVA444P9BE);
    LOGENUMVALUE(AV_PIX_FMT_YUVA444P9LE);
    LOGENUMVALUE(AV_PIX_FMT_YUVA420P10BE);
    LOGENUMVALUE(AV_PIX_FMT_YUVA420P10LE);
    LOGENUMVALUE(AV_PIX_FMT_YUVA422P10BE);
    LOGENUMVALUE(AV_PIX_FMT_YUVA422P10LE);
    LOGENUMVALUE(AV_PIX_FMT_YUVA444P10BE);
    LOGENUMVALUE(AV_PIX_FMT_YUVA444P10LE);
    LOGENUMVALUE(AV_PIX_FMT_YUVA420P16BE);
    LOGENUMVALUE(AV_PIX_FMT_YUVA420P16LE);
    LOGENUMVALUE(AV_PIX_FMT_YUVA422P16BE);
    LOGENUMVALUE(AV_PIX_FMT_YUVA422P16LE);
    LOGENUMVALUE(AV_PIX_FMT_YUVA444P16BE);
    LOGENUMVALUE(AV_PIX_FMT_YUVA444P16LE);
    LOGENUMVALUE(AV_PIX_FMT_VDPAU);
    LOGENUMVALUE(AV_PIX_FMT_XYZ12LE);
    LOGENUMVALUE(AV_PIX_FMT_XYZ12BE);
    LOGENUMVALUE(AV_PIX_FMT_NV16);
    LOGENUMVALUE(AV_PIX_FMT_NV20LE);
    LOGENUMVALUE(AV_PIX_FMT_NV20BE);
    LOGENUMVALUE(AV_PIX_FMT_0RGB);
    LOGENUMVALUE(AV_PIX_FMT_RGB0);
    LOGENUMVALUE(AV_PIX_FMT_0BGR);
    LOGENUMVALUE(AV_PIX_FMT_BGR0);
    LOGENUMVALUE(AV_PIX_FMT_YUVA444P);
    LOGENUMVALUE(AV_PIX_FMT_YUVA422P);
    LOGENUMVALUE(AV_PIX_FMT_YUV420P12BE);
    LOGENUMVALUE(AV_PIX_FMT_YUV420P12LE);
    LOGENUMVALUE(AV_PIX_FMT_YUV420P14BE);
    LOGENUMVALUE(AV_PIX_FMT_YUV420P14LE);
    LOGENUMVALUE(AV_PIX_FMT_YUV422P12BE);
    LOGENUMVALUE(AV_PIX_FMT_YUV422P12LE);
    LOGENUMVALUE(AV_PIX_FMT_YUV422P14BE);
    LOGENUMVALUE(AV_PIX_FMT_YUV422P14LE);
    LOGENUMVALUE(AV_PIX_FMT_YUV444P12BE);
    LOGENUMVALUE(AV_PIX_FMT_YUV444P12LE);
    LOGENUMVALUE(AV_PIX_FMT_YUV444P14BE);
    LOGENUMVALUE(AV_PIX_FMT_YUV444P14LE);
    LOGENUMVALUE(AV_PIX_FMT_GBRP12BE);
    LOGENUMVALUE(AV_PIX_FMT_GBRP12LE);
    LOGENUMVALUE(AV_PIX_FMT_GBRP14BE);
    LOGENUMVALUE(AV_PIX_FMT_GBRP14LE);
    LOGENUMVALUE(AV_PIX_FMT_GBRAP);
    LOGENUMVALUE(AV_PIX_FMT_GBRAP16BE);
    LOGENUMVALUE(AV_PIX_FMT_GBRAP16LE);
    LOGENUMVALUE(AV_PIX_FMT_YUVJ411P);
    LOGENUMVALUE(AV_PIX_FMT_BAYER_BGGR8);
    LOGENUMVALUE(AV_PIX_FMT_BAYER_RGGB8);
    LOGENUMVALUE(AV_PIX_FMT_BAYER_GBRG8);
    LOGENUMVALUE(AV_PIX_FMT_BAYER_GRBG8);
    LOGENUMVALUE(AV_PIX_FMT_BAYER_BGGR16LE);
    LOGENUMVALUE(AV_PIX_FMT_BAYER_BGGR16BE);
    LOGENUMVALUE(AV_PIX_FMT_BAYER_RGGB16LE);
    LOGENUMVALUE(AV_PIX_FMT_BAYER_RGGB16BE);
    LOGENUMVALUE(AV_PIX_FMT_BAYER_GBRG16LE);
    LOGENUMVALUE(AV_PIX_FMT_BAYER_GBRG16BE);
    LOGENUMVALUE(AV_PIX_FMT_BAYER_GRBG16LE);
    LOGENUMVALUE(AV_PIX_FMT_BAYER_GRBG16BE);
  default: os << "Unknown AVPixelFormat (" << static_cast<long>(obj) << ")";
  }
  return os;
};

std::ostream& operator<<(std::ostream& os, const AVCodec* obj) {
  if (obj) {
    os << '{'
      << "name=" << obj->name << ','
      << "long_name=" << obj->long_name << ','
      << "type=" << static_cast<int>(obj->type) << ','
      << "id=" << obj->id << ','
      << "max_lowres=" << static_cast<unsigned int>(obj->max_lowres) << ','
      << "capabilities=" << obj->capabilities;
    std::vector<std::string> caps;
    if (obj->capabilities & CODEC_CAP_DELAY) { caps.emplace_back("CODEC_CAP_DELAY"); }
    if (obj->capabilities & CODEC_CAP_SMALL_LAST_FRAME) { caps.emplace_back("CODEC_CAP_SMALL_LAST_FRAME"); }
    if (obj->capabilities & CODEC_CAP_SUBFRAMES) { caps.emplace_back("CODEC_CAP_SUBFRAMES"); }
    if (obj->capabilities & CODEC_CAP_CHANNEL_CONF) { caps.emplace_back("CODEC_CAP_CHANNEL_CONF"); }
    if (obj->capabilities & CODEC_CAP_FRAME_THREADS) { caps.emplace_back("CODEC_CAP_FRAME_THREADS"); }
    if (obj->capabilities & CODEC_CAP_SLICE_THREADS) { caps.emplace_back("CODEC_CAP_SLICE_THREADS"); }
    if (obj->capabilities & CODEC_CAP_AUTO_THREADS) { caps.emplace_back("CODEC_CAP_AUTO_THREADS"); }
    if (obj->capabilities & CODEC_CAP_VARIABLE_FRAME_SIZE) { caps.emplace_back("CODEC_CAP_VARIABLE_FRAME_SIZE"); }
    if (obj->capabilities & CODEC_CAP_INTRA_ONLY) { caps.emplace_back("CODEC_CAP_INTRA_ONLY"); }
    if (obj->capabilities & CODEC_CAP_LOSSLESS) { caps.emplace_back("CODEC_CAP_LOSSLESS"); }
    if (caps.size() > 0) {
      os << '(';
      auto first = true;
      for (auto c : caps) {
        os << (!first ? "," : "") << c;
        first = false;
      }
      os << ')';
    }
    os << ",pix_fmts=";
    if (!obj->pix_fmts) {
      os << '0';
    } else {
      os << '[';
      auto f = obj->pix_fmts;
      os << *f;
      os << ']';
    }
    os << '}';
  } else {
    os << '0';
  }
  return os;
}

std::ostream& operator<<(std::ostream& os, const AVCodecContext* obj) {
  if (obj) {
    os << '{'
      << "pix_fmt=" << obj->pix_fmt << ','
      << "time_base=" << obj->time_base << ','
      << "width=" << obj->width << ','
      << "height=" << obj->height << ','
      << "bitrate=" << obj->bit_rate << ','
      << "sample_rate=" << obj->sample_rate << ','
      << "channels=" << obj->channels << ','
      << "sample_fmt=" << obj->sample_fmt << ','
      << "has_b_frames=" << obj->has_b_frames << ','
      << "sample_aspect_ratio=" << obj->sample_aspect_ratio << ','
      << "debug=" << obj->debug << ','
      << "lowres=" << obj->lowres << ','
      << "coded_width=" << obj->coded_width << ','
      << "coded_height=" << obj->coded_height << ','
      << "request_channel_layout=" << obj->request_channel_layout << ','
      << "hwaccel=" << obj->hwaccel << ','
      << "ticks_per_frame=" << obj->ticks_per_frame << ','
      << "codec=" << obj->codec
      << '}';
  } else {
    os << '0';
  }
  return os;
}

std::ostream& operator<<(std::ostream& os, const AVFormatContext* obj) {
  if (obj) {
    os << '{';
    if (obj->av_class) {
      os << "avclass=" << obj->av_class->class_name << ',';
    }
    if (obj->iformat) {
      os << "iformat.name=" << obj->iformat->name << ','
        << "iformat.long_name=" << obj->iformat->long_name << ',';
    }
    if (obj->oformat) {
      os << "oformat.name=" << obj->oformat->name << ','
        << "oformat.long_name=" << obj->oformat->long_name << ',';
    }
    os << "nb_streams=" << obj->nb_streams << ','
      << "filename=" << obj->filename << ','
      << "ctx_flags=" << obj->ctx_flags << ','
      << "start_time=" << obj->start_time << ','
      << "duration=" << obj->duration << ','
      << "bit_rate=" << obj->bit_rate << ','
      << "packet_size=" << obj->packet_size << ','
      << "max_delay=" << obj->max_delay << ','
      << "flags=" << obj->flags << ','
      << "probesize=" << obj->probesize << ','
      << "video_codec_id=" << obj->video_codec_id << ','
      << "audio_codec_id=" << obj->audio_codec_id << ','
      << "subtitle_codec_id=" << obj->subtitle_codec_id << ','
      << "max_index_size=" << obj->max_index_size << ','
      << "max_picture_buffer=" << obj->max_picture_buffer << ','
      << "nb_chapters=" << obj->nb_chapters << ','
      << "debug=" << obj->debug << ','
      << "start_time_realtime=" << obj->start_time_realtime
      << '}';
  } else {
    os << '0';
  }
  return os;
}

std::ostream& operator<<(std::ostream& os, const AVStream* obj) {
  if (obj) {
    os << '{'
      << "index=" << obj->index << ','
      << "id=" << obj->id << ','
      << "codec=" << obj->codec << ','
      << "r_frame_rate=" << obj->r_frame_rate << ','
      << "time_base=" << obj->time_base << ','
      << "pts_wrap_bits=" << obj->pts_wrap_bits << ','
      << "discard=" << static_cast<int>(obj->discard) << ','
      << "start_time=" << obj->start_time << ','
      << "duration=" << obj->duration << ','
      << "cur_dts=" << obj->cur_dts << ','
      << "last_IP_duration=" << obj->last_IP_duration << ','
      << "last_IP_pts=" << obj->last_IP_pts << ','
      << "nb_index_entries=" << obj->nb_index_entries << ','
      << "index_entries_allocated_size=" << obj->index_entries_allocated_size << ','
      << "nb_frames=" << obj->nb_frames << ','
      << "disposition=" << obj->disposition << ','
      << "sample_aspect_ratio=" << obj->sample_aspect_ratio << ','
      << "avg_frame_rate=" << obj->avg_frame_rate << ','
      << "codec_info_nb_frames=" << obj->codec_info_nb_frames
      << '}';
  } else {
    os << '0';
  }
  return os;
}

std::ostream& operator<<(std::ostream& os, const AVPacket* obj) {
  if (obj) {
    os << '{'
      << "pts=" << obj->pts << ','
      << "dts=" << obj->dts << ','
      << "size=" << obj->size << ','
      << "stream_index=" << obj->stream_index << ','
      << "flags=" << obj->flags << ','
      << "side_data_elems=" << obj->side_data_elems << ','
      << "duration=" << obj->duration << ','
      << "pos=" << obj->pos
      << '}';
  } else {
    os << '0';
  }
  return os;
}

std::ostream& operator<<(std::ostream& os, const AVOutputFormat* obj) {
  if (obj) {
    os << '{'
      << "name=" << (obj->name ? obj->name : "0") << ','
      << "long_name=" << (obj->long_name ? obj->long_name : "0") << ','
      << "mime_type=" << (obj->mime_type ? obj->mime_type : "0") << ','
      << "extensions=" << (obj->extensions ? obj->extensions : "0") << ','
      << "flags=" << obj->flags << ',' // AVFMT_NOFILE, AVFMT_NEEDNUMBER, AVFMT_RAWPICTURE, AVFMT_GLOBALHEADER, AVFMT_NOTIMESTAMPS, AVFMT_VARIABLE_FPS, AVFMT_NODIMENSIONS, AVFMT_NOSTREAMS
      << "priv_data_size=" << obj->priv_data_size << ','
      << "audio_codec=" << obj->audio_codec << ','
      << "video_codec=" << obj->video_codec << ','
      << "subtitle_codec=" << obj->subtitle_codec
      << '}';
  } else {
    os << '0';
  }
  return os;
}

std::ostream& operator<<(std::ostream& os, const AVCodecID& obj) {
  for (auto i = 0; ; i++) {
    if (s_avCodecIdNames[i].avci == obj) {
      os << s_avCodecIdNames[i].name;
      break;
    } else if (s_avCodecIdNames[i].name == nullptr) {
      os << "Unknown AVCodecID (" << static_cast<long>(obj) << ")";
      break;
    }
  }
  return os;
}

std::ostream& operator<<(std::ostream& os, const AVSampleFormat& obj) {
  for (auto i = 0; ; i++) {
    if (s_avSampleFormatNames[i].avsf == obj) {
      os << s_avSampleFormatNames[i].name;
      break;
    } else if (s_avSampleFormatNames[i].name == nullptr) {
      os << "Unknown AVSampleFormat (" << static_cast<long>(obj) << ")";
      break;
    }
  }
  return os;
};

void mxp::logging::printAvError(const char *msg, int err) {
  char errbuf[AV_ERROR_MAX_STRING_SIZE];
  const char *errbuf_ptr = errbuf;

  if (av_strerror(err, errbuf, sizeof(errbuf)) < 0)
    errbuf_ptr = strerror(AVUNERROR(err));

  LOG_ERROR(msg, errbuf_ptr);
}
