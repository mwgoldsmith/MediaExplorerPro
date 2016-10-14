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

#ifndef IALBUMTRACK_H
#define IALBUMTRACK_H

#include "mediaexplorer/Types.h"

namespace mxp {

class IAlbumTrack {
public:
  virtual ~IAlbumTrack() = default;

  virtual int64_t Id() const = 0;

  /**
   * @brief artist Returns the artist, as tagged in the media.
   * This can be different from the associated media's artist.
   * For instance, in case of a featuring, Media::artist() might return
   * "Artist 1", while IAlbumTrack::artist() might return something like
   * "Artist 1 featuring Artist 2 and also artist 3 and a whole bunch of people"
   * @return
   */
  virtual ArtistPtr GetArtist() const = 0;

  virtual GenrePtr GetGenre() = 0;

  virtual unsigned int TrackNumber() = 0;

  virtual AlbumPtr GetAlbum() = 0;

  virtual MediaPtr GetMedia() = 0;

  /**
   * @return Which disc this tracks appears on (or 0 if unspecified)
   */
  virtual unsigned int DiscNumber() const = 0;
};

}

#endif // IALBUMTRACK_H
