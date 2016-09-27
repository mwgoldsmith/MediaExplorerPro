﻿/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#pragma once

#include <vector>

#include "Types.h"

namespace mxp {

class IPlaylist {
public:
  virtual ~IPlaylist() = default;
  virtual int64_t id() const = 0;
  virtual const std::string& name() const = 0;
  virtual bool setName(const std::string& name) = 0;
  virtual time_t creationDate() const = 0;
  virtual std::vector<MediaPtr> media() const = 0;
  ///
  /// \brief append Appends a media to a playlist
  /// The media will be the last element of a subsequent call to media()
  /// This is equivalent to calling add( media, 0 )
  /// \param media The media to add
  /// \return true on success, false on failure.
  ///
  virtual bool append(int64_t mediaId) = 0;
  ///
  /// \brief add Add a media to the playlist at the given position.
  /// Valid positions start at 1. 0 means appending.
  /// \param media The media to add
  /// \param position The position of this new media
  /// \return true on success, false on failure
  ///
  virtual bool add(int64_t mediaId, unsigned int position) = 0;
  ///
  /// \brief move Change the position of a media
  /// \param mediaId The media to move reorder
  /// \param position The new position within the playlist.
  /// 0 is an invalid value when moving.
  /// In case there is already a media at the given position, it will be placed after
  /// the media being moved. This will cascade to any media placed afterward.
  /// For instance, a playlist with <media,position> like
  /// [<1,1>, <2,2>, <3,3>] on which move(1, 2) is called will result in the playlist
  /// being changed to
  /// [<1,2>, <2,3>, <3,4>]
  /// \return true on success, false on failure
  ///
  virtual bool move(int64_t mediaId, unsigned int position) = 0;
  ///
  /// \brief remove Removes a media from the playlist
  /// \param mediaId The media to remove.
  /// \return true on success, false on failure
  ///
  virtual bool remove(int64_t mediaId) = 0;
};

}
