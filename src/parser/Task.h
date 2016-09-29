/*****************************************************************************
* Media Explorer
*****************************************************************************/

#pragma once

#include <string>
#include <memory>
#include <vector>

namespace mxp {

class Media;
class MediaFile;

namespace parser {

struct Task {
  enum class Status {
    // Default value.
    // Also, having success = 0 is not the best idea ever.
    Unknown,
    // All good
    Success,
    // Something failed, but it's not critical (For instance, no internet connection for a
    // module that uses an online database)
    Error,
    // We can't compute this file for now (for instance the file was on a network drive which
    // isn't connected anymore)
    TemporaryUnavailable,
    // Something failed and we won't continue
    Fatal
  };

  Task(std::shared_ptr<Media> media, std::shared_ptr<MediaFile> file)
    : media(media)
      , file(file)
      , currentService(0)
      , trackNumber(0)
      , discNumber(0)
      , discTotal(0)
      , episode(0)
      , duration(0) { }

  struct VideoTrackInfo {
    VideoTrackInfo(const std::string& fcc, float fps, unsigned int width, unsigned int height,
                   const std::string& language, const std::string& description)
      : fcc(fcc), fps(fps), width(width), height(height)
        , language(language), description(description) { }

    std::string fcc;
    float fps;
    unsigned int width;
    unsigned int height;
    std::string language;
    std::string description;
  };

  struct AudioTrackInfo {
    AudioTrackInfo(const std::string& fcc, unsigned int bitrate, unsigned int samplerate,
                   unsigned int nbChannels, const std::string& language, const std::string& description)
      : fcc(fcc), bitrate(bitrate), samplerate(samplerate), nbChannels(nbChannels),
        language(language), description(description) { }

    std::string fcc;
    unsigned int bitrate;
    unsigned int samplerate;
    unsigned int nbChannels;
    std::string language;
    std::string description;
  };

  std::shared_ptr<Media> media;
  std::shared_ptr<MediaFile> file;
  unsigned int currentService;

  std::vector<VideoTrackInfo> VideoTracks;
  std::vector<AudioTrackInfo> AudioTracks;

  std::string albumArtist;
  std::string artist;
  std::string albumName;
  std::string title;
  std::string artworkMrl;
  std::string genre;
  std::string ReleaseDate;
  std::string showName;
  int trackNumber;
  int discNumber;
  int discTotal;
  int episode;
  int64_t duration;
};

}}
