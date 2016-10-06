#include "FormatService.h"
#include "Media.h"
#include "mediaexplorer/ILogger.h"

bool mxp::FormatService::initialize() {
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

mxp::parser::Task::Status mxp::FormatService::run(parser::Task & task) {
  auto media = task.media;
  auto file = task.file;

  // FIXME: This is now becoming an invalid predicate
  if(media->GetDuration() != -1) {
    LOG_INFO(file->mrl(), " was already parsed");
    return parser::Task::Status::Success;
  }

  LOG_INFO("Parsing ", file->mrl());
  auto chrono = std::chrono::steady_clock::now();

  auto parserMedia = m_ml->GetParserMediaFactory()->CreateParserMedia(file->mrl());

  auto duration = std::chrono::steady_clock::now() - chrono;
  LOG_DEBUG("Parsing done in ", std::chrono::duration_cast<std::chrono::microseconds>(duration).count(), "µs");

  return parser::Task::Status::Success;
}

