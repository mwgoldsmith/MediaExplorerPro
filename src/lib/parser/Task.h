/*****************************************************************************
* Media Explorer
*****************************************************************************/

#pragma once

#include "stdafx.h"
#if HAVE_CONFIG_H
# include "config.h"
#endif

#include "mediaexplorer/Common.h"
#include <mediaexplorer/MediaType.h>
#include <mediaexplorer/IMedia.h>

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

  /**
  * @param name  The key name of the metadata entry
  * @param value The value of the metadata entry
  */
  struct MetadataInfo {
    MetadataInfo(const mstring& name, const mstring& value)
      : Name(name)
      , Value(value) { }

    const mstring Name;
    const mstring Value;
  };

  /**
  * @param index     The index of the stream.
  * @param type      The MediaType of the stream.
  * @param codecName The name of the codec used by the stream.
  *
  * The codec name is assumed to be populated from the AVCodec
  * structure. As such, the member "name" is of type 'char',
  * regardless of the type of mchar and mstring for the current
  * build. Therefor, it will always be narrow and should be
  * converted to an mstring value.
  */
  struct StreamInfo {
    StreamInfo(unsigned int index, MediaType type, const char* codecName)
      : Metadata{}
      , CodecName(to_mstring(codecName))
      , Index(index)
      , Type(type) { }

    std::vector<MetadataInfo> Metadata;
    const mstring             CodecName;
    unsigned int              Index;
    MediaType                 Type;
  };

  /**
  * @param index     
  * @param fcc
  * @param fps
  * @param width
  * @param height
  */
  struct VideoTrackInfo {
    VideoTrackInfo(unsigned int index, const mstring& fcc, float fps, unsigned int width, unsigned int height)
      : Fcc(fcc)
      , Language{}
      , Description{}
      , Index(index)
      , Width(width)
      , Height(height)
      , Fps(fps) { }
     
    const mstring Fcc;
    const mstring Language;
    const mstring Description;
    unsigned int  Index;
    unsigned int  Width;
    unsigned int  Height;
    float         Fps;
  };

  /**
  * @param index
  * @param fcc
  * @param bitrate
  * @param samplerate
  * @param numChannels
  */
  struct AudioTrackInfo {
    AudioTrackInfo(unsigned int index, const mstring& fcc, unsigned int bitrate, unsigned int samplerate, unsigned int numChannels)
      : Fcc(fcc)
      , Language({})
      , Description({})
      , Index(index)
      , Bitrate(bitrate)
      , Samplerate(samplerate)
      , NumChannels(numChannels) { }

    mstring      Fcc;
    mstring      Language;
    mstring      Description;
    unsigned int Index;
    unsigned int Bitrate;
    unsigned int Samplerate;
    unsigned int NumChannels;
  };

  struct ChapterInfo {
    ChapterInfo(int64_t start, int64_t end)
      : Start(start)
      , End(end) {
    }

    int64_t Start;
    int64_t End;
  };

  Task(std::shared_ptr<Media> media, std::shared_ptr<MediaFile> mediaFile)
    : Media(media)
    , MediaFile(mediaFile)
    , Type(IMedia::Type::UnknownType)
    , Duration(0)
    , Episode(0)
    , TrackNumber(0)
    , CurrentService(0)
    , artworkMrl{}
    , releaseDate{}
    , discNumber(0)
    , discTotal(0) { }

  std::vector<MetadataInfo>   Metadata;
  std::vector<StreamInfo>     Streams;
  std::vector<VideoTrackInfo> VideoTracks;
  std::vector<AudioTrackInfo> AudioTracks;
  std::shared_ptr<Media>      Media;
  std::shared_ptr<MediaFile>  MediaFile;
  mxp::IMedia::Type           Type;
  mstring                     ContainerName;
  mstring                     Title;
  mstring                     Genre;
  mstring                     ShowName;
  mstring                     Artist;
  mstring                     AlbumArtist;
  mstring                     AlbumName;
  int64_t                     Duration;
  int                         Episode;
  int                         TrackNumber;
  unsigned int                CurrentService;

  mstring artworkMrl;
  mstring releaseDate;
  int discNumber;
  int discTotal;
};

}}
