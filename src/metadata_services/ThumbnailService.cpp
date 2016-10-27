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
  return mxp::utils::file::ConcatPath(path, name);
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

  auto filename = file->mrl();
  LOG_INFO( "Generating thumbnail for: ", filename);

  ImagePtr image = nullptr;
  JpegImageContainer jpeg;

  auto ctx = av::MediaController::CreateContext(filename);
  if(ctx != nullptr) {
    if(av::MediaController::OpenStream(ctx, StreamType::Video, 0)) {
      auto position = 10.0;
      if(av::MediaController::Seek(ctx, position)) {
        image = av::MediaController::ReadFrame(ctx);
        if(image == nullptr) {
          LOG_ERROR("Failed to read frame from media.");
        }
      } else {
        LOG_ERROR("Failed to seek to position in media: ", position);
      }
    } else {
      LOG_ERROR("Failed to open Video stream in media.");
    }

    if(image != nullptr) {
      uint32_t width;
      uint32_t height;

      m_ml->GetSettingInt(SettingsKey::ThumbnailHeight, height);
      m_ml->GetSettingInt(SettingsKey::ThumbnailWidth, width);
      if(width >= 0 && height >= 0 && width <= 3200 && height <= 3200) {
        if (!image->Scale(width, height, true)) {
          LOG_ERROR("Failed to scale image to dimensions: ", width, "x", height);
        }
      }

      mstring path;
      m_ml->GetSettingString(SettingsKey::ThumbnailPath, path);
      filename = GetThumbnailFilename(path, filename, jpeg);
    }
  }

  if(image != nullptr) {
    if(jpeg.Compress(image) == true) {
      if(jpeg.Save(filename) == true) {
        media->SetThumbnail(filename);
        media->Save();
        return parser::Task::Status::Success;
      } else {
        LOG_ERROR("Failed to save JPEG image as: ", filename);
      }
    } else {
      LOG_ERROR("Failed to compress thumbnail image as JPEG data");
    }
  }

  return parser::Task::Status::Fatal;
}

