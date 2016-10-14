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

#ifndef ALBUM_H
#define ALBUM_H

#include "database/DatabaseHelpers.h"
#include "mediaexplorer/IAlbum.h"
#include "utils/Cache.h"
#include "mediaexplorer/Types.h"
#include "Types.h"

namespace mxp {

class Album;
class AlbumTrack;
class Artist;
class Media;

namespace policy {
struct AlbumTable {
  static const std::string Name;
  static const std::string PrimaryKeyColumn;
  static int64_t Album::*const PrimaryKey;
};
}

class Album : public IAlbum, public DatabaseHelpers<Album, policy::AlbumTable> {
public:
  Album(MediaExplorerPtr ml, sqlite::Row& row);
  Album(MediaExplorerPtr ml, const std::string& title);
  Album(MediaExplorerPtr ml, const Artist* artist);

  virtual int64_t Id() const override;
  virtual const std::string& Title() const override;
  virtual unsigned int ReleaseYear() const override;
  /**
   * @brief setReleaseYear Updates the release year
   * @param date The desired date.
   * @param force If force is true, the date will be applied no matter what.
   *              If force is false, the date will be applied if it never was
   *              applied before. Otherwise, setReleaseYear() will restore the release
   *              date to 0.
   * @return
   */
  bool SetReleaseYear(unsigned int date, bool force);
  virtual const std::string& ShortSummary() const override;
  bool SetShortSummary(const std::string& summary);
  virtual const std::string& artworkMrl() const override;
  bool setArtworkMrl(const std::string& artworkMrl);
  virtual std::vector<MediaPtr> Tracks(SortingCriteria sort, bool desc) const override;
  virtual std::vector<MediaPtr> Tracks(GenrePtr genre, SortingCriteria sort, bool desc) const override;
  ///
  /// \brief cachedTracks Returns a cached list of tracks
  /// This has no warranty of ordering, validity, or anything else.
  /// \return An unordered-list of this album's tracks
  ///
  std::vector<MediaPtr> CachedTracks() const;
  ///
  /// \brief addTrack Add a track to the album.
  /// This will modify the media, but *not* save it.
  /// The media will be added to the tracks cache.
  ///
  std::shared_ptr<AlbumTrack> AddTrack(std::shared_ptr<Media> media, unsigned int trackNb, unsigned int discNumber);
  unsigned int nbTracks() const override;
  unsigned int Duration() const override;

  virtual ArtistPtr AlbumArtist() const override;
  bool SetAlbumArtist(std::shared_ptr<Artist> artist);
  virtual std::vector<ArtistPtr> Artists(bool desc) const override;
  bool AddArtist(std::shared_ptr<Artist> artist);
  bool RemoveArtist(Artist* artist);

  static bool CreateTable(DBConnection dbConnection);
  static bool CreateTriggers(DBConnection dbConnection);
  static std::shared_ptr<Album> create(MediaExplorerPtr ml, const std::string& title);
  static std::shared_ptr<Album> createUnknownAlbum(MediaExplorerPtr ml, const Artist* artist);
  ///
  /// \brief search search for an album, through its albumartist or title
  /// \param pattern A pattern representing the title, or the name of the main artist
  /// \return
  ///
  static std::vector<AlbumPtr> Search(MediaExplorerPtr ml, const std::string& pattern);
  static std::vector<AlbumPtr> fromArtist(MediaExplorerPtr ml, int64_t artistId, SortingCriteria sort, bool desc);
  static std::vector<AlbumPtr> fromGenre(MediaExplorerPtr ml, int64_t genreId, SortingCriteria sort, bool desc);
  static std::vector<AlbumPtr> ListAll(MediaExplorerPtr ml, SortingCriteria sort, bool desc);

private:
  static std::string OrderTracksBy(SortingCriteria sort, bool desc);
  static std::string OrderBy(SortingCriteria sort, bool desc);

protected:
  MediaExplorerPtr     m_ml;
  int64_t              m_id;
  std::string          m_title;
  int64_t              m_artistId;
  unsigned int         m_releaseYear;
  std::string          m_shortSummary;
  std::string          m_artworkMrl;
  unsigned int         m_nbTracks;
  int64_t              m_duration;
  bool                 m_isPresent;

  mutable Cache<std::vector<MediaPtr>>   m_tracks;
  mutable Cache<std::shared_ptr<Artist>> m_albumArtist;

  friend struct policy::AlbumTable;
};

}

#endif // ALBUM_H
