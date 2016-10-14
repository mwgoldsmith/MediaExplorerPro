/*****************************************************************************
* Media Explorer
*****************************************************************************/

#if HAVE_CONFIG_H
# include "config.h"
#endif

#include "FormatService.h"
#include "Media.h"
#include "MediaFile.h"
#include "Metadata.h"
#include "parser/ParserMedia.h"
#include "mediaexplorer/ILogger.h"
#include <av/AvController.h>
#include <Stream.h>

bool mxp::FormatService::Initialize() {
  //m_unknownArtist = Artist::Fetch(m_ml, UnknownArtistID);
  //if (m_unknownArtist == nullptr)
  // return m_unknownArtist != nullptr;
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
    LOG_INFO(file->mrl(), " was already parsed");
    return parser::Task::Status::Success;
  }

  LOG_INFO("Parsing ", file->mrl());
  auto chrono = std::chrono::steady_clock::now();

  auto ifile = m_ml->GetFileSystem()->CreateFileFromPath(file->mrl());
  auto parserMedia = parser::ParserMedia(m_ml, task);

  // Set the media container
  auto container = av::AvController::FindContainer(task.ContainerName);
  media->SetContainer(container);

  // Create or update all metadata
  auto metadata = media->GetMetadata();
  UpdateMetadata(media, metadata, task.Metadata);

  auto streams = Stream::FindByMedia(m_ml, media->Id());
  for(const auto& tag : task.Streams) {
    auto codec = av::AvController::FindCodec(tag.CodecName, tag.Type);
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

  auto duration = std::chrono::steady_clock::now() - chrono;
  LOG_DEBUG("Parsing done in ", std::chrono::duration_cast<std::chrono::microseconds>(duration).count(), "µs");

  return parser::Task::Status::Success;
}

