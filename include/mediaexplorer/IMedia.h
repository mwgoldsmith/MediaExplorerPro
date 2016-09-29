/*****************************************************************************
 * Media Library
 *****************************************************************************
 * Copyright (C) 2015 Hugo Beauzée-Luyssen, Videolabs
 *
 * Authors: Hugo Beauzée-Luyssen<hugo@beauzee.fr>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301, USA.
 *****************************************************************************/

#pragma once

#include <vector>
#include <memory>

#include "mediaexplorer/IMediaExplorer.h"

namespace mxp {

class IAlbumTrack;
class IShowEpisode;
class ITrackInformation;

class IMedia {
public:
  enum class Type : uint8_t {
    VideoType,
    AudioType,
    UnknownType
  };
  enum class SubType : uint8_t {
    Unknown,
    ShowEpisode,
    Movie,
    AlbumTrack,
  };

  virtual ~IMedia() = default;

  virtual int64_t id() const = 0;
  virtual Type type() = 0;
  virtual SubType subType() const = 0;
  virtual const std::string& title() const = 0;
  //virtual AlbumTrackPtr albumTrack() const = 0;
  /**
   * @brief duration Returns the media duration in ms
   */
  virtual int64_t duration() const = 0;
  //virtual ShowEpisodePtr showEpisode() const = 0;
  virtual int PlayCount() const = 0;
  virtual bool IncreasePlayCount() = 0;
  virtual const std::vector<MediaFilePtr>& Files() const = 0;
  /**
   *  @brief progress Returns the progress, in the [0;1] range
   */
  virtual float progress() const = 0;
  virtual bool SetProgress(float progress) = 0;
  /**
   *  @brief rating The media rating, or -1 if unset.
   *  It is up to the application to determine the values it wishes to use.
   *  No value is enforced, and any positive value (less or equal to INT32_MAX)
   *  will be accepted.
   */
  virtual int rating() const = 0;
  virtual bool SetRating(int rating) = 0;
  virtual bool isFavorite() const = 0;
  virtual bool SetFavorite(bool favorite) = 0;
  virtual bool AddLabel(LabelPtr label) = 0;
  virtual bool RemoveLabel(LabelPtr label) = 0;
  virtual std::vector<LabelPtr> labels() = 0;
  //virtual MoviePtr movie() const = 0;
  virtual std::vector<VideoTrackPtr> VideoTracks() = 0;
  virtual std::vector<AudioTrackPtr> AudioTracks() = 0;
  /**
   *  @brief thumbnail Returns the path of a thumbnail for this media
   *  @return A path, relative to the thumbnailPath configured when initializing
   *   The media library
   */
  virtual const std::string& Thumbnail() = 0;
  virtual time_t InsertionDate() const = 0;
  virtual time_t ReleaseDate() const = 0;
};

}
