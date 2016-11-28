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
#include "av/MediaController.h"
#include "av/StreamType.h"
#include "mediaexplorer/ILogger.h"
#include "image/Image.h"
#include "image/JpegImageContainer.h"
#include "utils/Filename.h"

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

std::string GetThumbnailFilename(const mstring& path, const mstring& mrl, const mxp::IImageContainer& container) {
  auto name = mxp::utils::file::toAbsolutePath(mrl);
  name = mxp::utils::file::fileName(name);

  auto pos = name.find_last_of('.');;
  if(pos != std::string::npos)
    name = name.substr(0, pos + 1);

  name = name + "." + container.GetExtension();
  return mxp::utils::file::concatPath(path, name);
}

mxp::parser::Task::Status mxp::ThumbnailService::Run(parser::Task & task) {
  auto media = task.Media;
  auto file = task.MediaFile;

  if (task.Type == MediaType::Unknown) {
    // If we don't know the media type yet, it actually looks more like a bug
    // since this should run after media type deduction, and not run in case
    // that step fails.
    return parser::Task::Status::Fatal;
  } else if (task.Type!= MediaType::Video) {
    // There's no point in generating a thumbnail for a non-video media.
    return parser::Task::Status::Success;
  } else if (media->Thumbnail().empty() == false) {
    LOG_INFO(media->Thumbnail(), " already has a thumbnail" );
    return parser::Task::Status::Success;
  }

  auto filename = file->GetMrl();
  LOG_INFO( "Generating thumbnail for: ", filename);

  ImagePtr image = nullptr;
  JpegImageContainer jpeg;

  auto ctx = av::MediaController::CreateContext(filename);
  if (ctx == nullptr) {
    LOG_ERROR("Failed to create MediaController context.");
    return parser::Task::Status::Fatal;
  }

  if(av::MediaController::OpenStream(ctx, StreamType::Video, 0)) {
    auto position = 10.0;
    if(av::MediaController::Seek(ctx, position)) {
      image = av::MediaController::ReadFrame(ctx);
      if(image == nullptr) {
        LOG_ERROR("Failed to read frame from media.");
        return parser::Task::Status::Fatal;
      }
    } else {
      LOG_ERROR("Failed to seek to position in media: ", position);
      return parser::Task::Status::Fatal;
    }
  } else {
    LOG_ERROR("Failed to open Video stream in media.");
    return parser::Task::Status::Fatal;
  }

  uint32_t width;
  uint32_t height;

  m_ml->GetSettingInt(SettingsKey::ThumbnailHeight, height);
  m_ml->GetSettingInt(SettingsKey::ThumbnailWidth, width);
  if(width >= 0 && height >= 0 && width <= 3200 && height <= 3200) {
    if (!image->Scale(width, height, true)) {
      LOG_ERROR("Failed to scale image to dimensions: ", width, "x", height);
      return parser::Task::Status::Fatal;
    }
  }

  mstring path;
  m_ml->GetSettingString(SettingsKey::ThumbnailPath, path);
  if (!utils::file::isExistingDirectory(path)) {
    if (!utils::file::createDirectory(path)) {
      LOG_ERROR("Failed to create thumbnail directory: ", path);
      return parser::Task::Status::Fatal;
    }
  }

  if(jpeg.Compress(image) == true) {
    filename = GetThumbnailFilename(path, media->GetGuid().ToString(), jpeg);
    if(jpeg.Save(filename) == true) {
      media->SetType(task.Type);
      media->SetThumbnail(filename);
      media->Save();

      return parser::Task::Status::Success;
    } else {
      LOG_ERROR("Failed to save JPEG image as: ", filename);
    }
  } else {
    LOG_ERROR("Failed to compress thumbnail image as JPEG data");
  }

  return parser::Task::Status::Fatal;
}

