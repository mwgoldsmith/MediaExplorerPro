
/*****************************************************************************
* Media Explorer
*****************************************************************************/

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
}

#include "mediaexplorer/Types.h"

#include <assert.h>
#include "Metadata.h"
#include "Media.h"
#include "MediaFile.h"
#include "Stream.h"
#include "logging/Logger.h"
#include "parser/ParserMedia.h"
#include "parser/Task.h"
#include <av/AvController.h>

void print_error(const char *msg, int err) {
  char errbuf[128];
  const char *errbuf_ptr = errbuf;

  if(av_strerror(err, errbuf, sizeof(errbuf)) < 0)
    errbuf_ptr = strerror(AVUNERROR(err));

  LOG_ERROR(msg, errbuf_ptr);
}


mxp::parser::ParserMedia::ParserMedia(MediaExplorerPtr ml, parser::Task& task)
  : m_ml(ml) {
  AVFormatContext*   fmtCtx=nullptr;

  auto file = m_ml->GetFileSystem()->CreateFileFromPath(task.MediaFile->mrl());
  auto filename = file->FullPath().c_str();
  LOG_DEBUG("Parsing media for file '", filename, "'");

  try {
    int err;
    if((err = avformat_open_input(&fmtCtx, filename, nullptr, nullptr)) < 0) {
      print_error("Failed to create media container: ", err);
    } else if((err = avformat_find_stream_info(fmtCtx, nullptr)) < 0) {
      print_error("Cannot find stream information: ", err);
    } else {
      task.ContainerName = to_mstring(fmtCtx->iformat->name);
      task.Duration = fmtCtx->duration;

      AVDictionaryEntry *tag = nullptr;
      while((tag = av_dict_get(fmtCtx->metadata, "", tag, AV_DICT_IGNORE_SUFFIX))) {
        LOG_TRACE("Metadata: ", tag->key, "=", tag->value);
        auto name = to_mstring(tag->key);
        auto value = to_mstring(tag->value);
        task.Metadata.emplace_back(name, value);

        if(name.compare("album_artist") == 0) {
          task.AlbumArtist = value;
        } else if(name.compare("artist") == 0) {
          task.Artist = value;
        } else if(name.compare("genre") == 0) {
          task.Genre = value;
        } else if(name.compare("title") == 0) {
          task.Title = value;
        } else if(name.compare("show") == 0) {
          task.ShowName = value;
        } else if(name.compare("track") == 0) {
          task.AlbumArtist = value;
        }
      }


      for(unsigned int i = 0; i < fmtCtx->nb_streams; i++) {
        auto context = fmtCtx->streams[i]->codec;
        
        if(!((context->codec_type == AVMEDIA_TYPE_VIDEO) || (context->codec_type == AVMEDIA_TYPE_AUDIO) || (context->codec_type == AVMEDIA_TYPE_SUBTITLE))) {
          LOG_WARN("Unsupported media stream type '", av_get_media_type_string(context->codec_type), "' in '", filename, "'. Ignoring.");
        } else {
          auto decoder = avcodec_find_decoder(context->codec_id);
          auto type = MediaType::None;

          if(decoder == nullptr) {
            LOG_ERROR("Unsupported codec: ", avcodec_get_name(context->codec_id));
          } else {
            type = av::GetMediaType(context->codec_type);
            task.Streams.emplace_back(i, type, decoder->name);
            auto &stream = task.Streams.at(task.Streams.size() - 1);

            while((tag = av_dict_get(fmtCtx->streams[i]->metadata, "", tag, AV_DICT_IGNORE_SUFFIX))) {
              LOG_TRACE("Metadata: ", tag->key, "=", tag->value);
              stream.Metadata.emplace_back(mstring(tag->key), mstring(tag->value));
            }
          }

          if(type == MediaType::Video) {
            auto fps = static_cast<float>(context->framerate.num) / static_cast<float>(context->framerate.den);
            std::string fcc(reinterpret_cast<const char*>(&context->codec_tag), 4);
            task.VideoTracks.emplace_back(i, fcc, fps, context->width, context->height);
          } else if(type == MediaType::Audio) {
            std::string fcc(reinterpret_cast<const char*>(&context->codec_tag), 4);
            task.AudioTracks.emplace_back(i, fcc, static_cast<unsigned int>(context->bit_rate), static_cast<int>(context->sample_rate), static_cast<int>(context->channels));
          }

          avcodec_close(context);
        }
      }

      avformat_close_input(&fmtCtx);
    }
  } catch (std::exception & ex) {
    LOG_ERROR(ex.what());
  }
}

/*


mxp::parser::ParserMedia::ParserMedia(MediaExplorerPtr ml, std::shared_ptr<Media> media, const std::shared_ptr<mxp::fs::IFile> file)
: m_file(file)
, m_ml(ml)
, m_context(std::make_unique<Context>()) {
assert(file != nullptr && file.get() != nullptr);

auto filename = m_file->FullPath().c_str();
LOG_DEBUG("Parsing media for file '", filename, "'");

// Set container info
// Set container metadata info
// Set streams info
//   Set stream codec info
//   Set stream metadata info

int err;
if((err = avformat_open_input(&fmtCtx, filename, nullptr, nullptr)) < 0) {
print_error("Failed to create media container: ", err);
} else if((err = avformat_find_stream_info(fmtCtx, nullptr)) < 0) {
print_error("Cannot find stream information: ", err);
} else {
auto fmt = fmtCtx->iformat;
m_context->m_container = av::AvController::FindContainer(fmt->name);
m_context->m_numStreams = fmtCtx->nb_streams;

LOG_INFO("Media file '", filename, "' contains ", m_context->m_numStreams, " streams.");

UpdateMetadata(fmtCtx->metadata, m_context->m_containerMeta);
UpdateStreams(ml, media);
}
}


bool UpsertMetadata(mxp::MetadataPtr m, AVDictionaryEntry *tag) {
  if(m->GetName().compare(tag->key) == 0) {
    if(m->GetValue().compare(tag->value) != 0) {
      m->SetValue(tag->value);
    }

    return true;
  }
  
  return false;
}

void mxp::parser::ParserMedia::UpdateMetadata(std::shared_ptr<Media> media) {
  AVDictionaryEntry *tag = nullptr;

  auto metadata = media->GetMetadata();
  while((tag = av_dict_get(fmtCtx->metadata, "", tag, AV_DICT_IGNORE_SUFFIX))) {
    LOG_TRACE("Metadata: ", tag->key, "=", tag->value);

    auto exists = false;
    for(auto const& m : metadata) {
      exists = UpsertMetadata(m, tag);
      if(exists) break;
    }

    if(!exists) {
      auto item = mxp::Metadata::Create(m_ml, tag->key, tag->value);
      media->AddMetadata(item->Id());
    }
  }
}

void mxp::parser::ParserMedia::UpdateMetadata(int index, StreamPtr stream) {
  AVDictionaryEntry *tag = nullptr;

  auto metadata = stream->GetMetadata();
  while((tag = av_dict_get(fmtCtx->streams[index]->metadata, "", tag, AV_DICT_IGNORE_SUFFIX))) {
    LOG_TRACE("Metadata: ", tag->key, "=", tag->value);

    auto exists = false;
    for(auto const& m : metadata) {
      exists = UpsertMetadata(m, tag);
      if(exists) break; 
    }

    if(!exists) {
      auto item = mxp::Metadata::Create(m_ml, tag->key, tag->value);
      stream->AddMetadata(item->Id());
    }
  }
}

void mxp::parser::ParserMedia::UpdateStreams(MediaExplorerPtr ml, std::shared_ptr<Media> media) {
  auto filename = m_file->FullPath().c_str();

  for(unsigned int i = 0; i < fmtCtx->nb_streams; i++) {
    auto context = fmtCtx->streams[i]->codec;
    if(!(context->codec_type == AVMEDIA_TYPE_VIDEO) || (context->codec_type == AVMEDIA_TYPE_AUDIO) || (context->codec_type == AVMEDIA_TYPE_SUBTITLE)) {
      LOG_WARN("Unsupported media stream type '", av_get_media_type_string(context->codec_type), "' in '", filename, "'. Ignoring.");
      continue;
    }

    auto type = mxp::av::GetMediaType(context->codec_type);
    if(type == MediaType::Video) {
      auto fps = static_cast<float>(context->framerate.num) / static_cast<float>(context->framerate.den);
      std::string fcc(reinterpret_cast<const char*>(&context->codec_tag), 4);
      AddVideoStream(i, context->width, context->height, fps, fcc);
    }


    auto decoder = avcodec_find_decoder(context->codec_id);
    if(decoder == nullptr) {
      LOG_ERROR("Unsupported codec: ", avcodec_get_name(context->codec_id));
      continue;
    }

    LOG_DEBUG("Stream[", i, "] = { '", filename, "', ", av_get_media_type_string(context->codec_type), ", ", avcodec_get_name(context->codec_id), " }");

    //auto type = mxp::av::GetMediaType(decoder->type);
    auto codec = av::AvController::FindCodec(decoder->name, type);
    auto stream = mxp::Stream::Create(ml, codec, i, media, type);

    UpdateMetadata(i, stream);

    avcodec_close(context);
  }

}
*/