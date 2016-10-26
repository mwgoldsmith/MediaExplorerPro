/*****************************************************************************
* Media Explorer
*****************************************************************************/

#include "stdafx.h"
#if HAVE_CONFIG_H
# include "config.h"
#endif

#include "ThumbnailService.h"
#include "Media.h"
#include "MediaFile.h"
#include "Metadata.h"
#include "mediaexplorer/ILogger.h"
#include "parser/ParserMedia.h"
#include <av/MediaController.h>
#include <av/StreamType.h>
#include <image/JpegImage.h>

bool mxp::ThumbnailService::Initialize() {
  return true;
}

const char* mxp::ThumbnailService::Name() const {
  return "Thumbnailer";
}

uint8_t mxp::ThumbnailService::nbThreads() const {
  //    auto nbProcs = std::thread::hardware_concurrency();
  //    if (nbProcs == 0)
  //        return 1;
  //    return nbProcs;
  // Let's make this code thread-safe first :)
  return 1;
}

mxp::parser::Task::Status mxp::ThumbnailService::Run(parser::Task & task) {
  auto media = task.Media;
  auto file = task.MediaFile;

  if (task.Type == IMedia::Type::UnknownType) {
    // If we don't know the media type yet, it actually looks more like a bug
    // since this should run after media type deduction, and not run in case
    // that step fails.
    return parser::Task::Status::Fatal;
  } else if (task.Type!= IMedia::Type::VideoType) {
    // There's no point in generating a thumbnail for a non-video media.
    return parser::Task::Status::Success;
  } else if (media->Thumbnail().empty() == false) {
    LOG_INFO(media->Thumbnail(), " already has a thumbnail" );
    return parser::Task::Status::Success;
  }

  LOG_INFO( "Generating ", file->mrl(), " thumbnail..." );
  auto ctx = av::MediaController::CreateContext(file->mrl());
  av::MediaController::OpenStream(ctx, StreamType::Video, 0);
  av::MediaController::Seek(ctx, 10.0);
  auto image = av::MediaController::ReadFrame(ctx);
  if(image != nullptr) {
    JpegImage jpeg;
    auto f = m_ml->GetFileSystem()->CreateFileFromPath(file->mrl());
    jpeg.save(image, f->GetName() + "." + jpeg.GetExtension());
  }
  
  return parser::Task::Status::Success;
}

