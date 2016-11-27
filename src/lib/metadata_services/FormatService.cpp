/*****************************************************************************
* Media Explorer
*****************************************************************************/

#include "stdafx.h"
#if HAVE_CONFIG_H
# include "config.h"
#endif

#include "FormatService.h"
#include "Media.h"
#include "MediaFile.h"
#include "Metadata.h"
#include "Stream.h"
#include "av/MediaController.h"
#include "mediaexplorer/ILogger.h"
#include "parser/ParserMedia.h"

bool mxp::FormatService::Initialize() {
  return true;
}

const char* mxp::FormatService::Name() const {
  return "Format";
}

uint8_t mxp::FormatService::nbThreads() const {
  //    auto nbProcs = std::thread::hardware_concurrency();
  //    if (nbProcs == 0)
  //        return 1;
  //    return nbProcs;
  // Let's make this code thread-safe first :)
  return 1;
}

bool mxp::FormatService::UpsertMetadata(mxp::MetadataPtr m, const mxp::parser::Task::MetadataInfo& tag) {
  if(m->GetName().compare(tag.Name) == 0) {
    if(m->GetValue().compare(tag.Value) != 0) {
      m->SetValue(tag.Value);
    }

    return true;
  }

  return false;
}

mxp::parser::Task::Status mxp::FormatService::Run(parser::Task & task) {
  auto media = task.Media;
  auto file = task.MediaFile;

  // FIXME: This is now becoming an invalid predicate
  if(media->GetDuration() != -1) {
    LOG_INFO(file->GetMrl(), " was already parsed");
    return parser::Task::Status::Success;
  }

  LOG_INFO("Parsing ", file->GetMrl());
  auto chrono = std::chrono::steady_clock::now();

  auto ifile = m_ml->GetFileSystem()->CreateFileFromPath(file->GetMrl());
  auto parserMedia = parser::ParserMedia(m_ml, task);

  // Set the media container
  auto container = av::MediaController::FindContainer(task.ContainerName);
  media->SetContainer(container);

  // Create or update all metadata
  auto metadata = media->GetMetadata();
  UpdateMetadata(media, metadata, task.Metadata);

  // FIXME: Find a way to determine the type of media
  auto type = IMedia::Type::UnknownType;

  auto streams = Stream::FindByMedia(m_ml, media->Id());
  for(const auto& tag : task.Streams) {
    if (tag.Type == MediaType::Video) {
      type = IMedia::Type::VideoType;
    } else if (type != IMedia::Type::VideoType && tag.Type == MediaType::Audio) {
      type = IMedia::Type::AudioType;
    }

    auto codec = av::MediaController::FindCodec(tag.CodecName, tag.Type);
    StreamPtr stream = nullptr;

    for(const auto& s : streams) {
      if(s->GetIndex() == tag.Index) {
        stream = s;
        break;
      }
    }

    if(stream == nullptr) {
      stream = mxp::Stream::Create(m_ml, codec, tag.Index, media, tag.Type);
    }

    metadata = stream->GetMetadata();
    UpdateMetadata(stream, metadata, tag.Metadata);
  }

  task.Type = type;
  media->Save();

  auto duration = std::chrono::steady_clock::now() - chrono;
  LOG_DEBUG("Parsing done in ", std::chrono::duration_cast<std::chrono::microseconds>(duration).count(), "µs");

  return parser::Task::Status::Success;
}

