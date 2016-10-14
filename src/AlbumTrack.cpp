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

#if HAVE_CONFIG_H
# include "config.h"
#endif

#include "AlbumTrack.h"
#include "Album.h"
#include "Artist.h"
#include "Media.h"
#include "Genre.h"
#include "database/SqliteTools.h"
#include "logging/Logger.h"

namespace mxp {

const std::string policy::AlbumTrackTable::Name = "AlbumTrack";
const std::string policy::AlbumTrackTable::PrimaryKeyColumn = "id_track";
int64_t AlbumTrack::* const policy::AlbumTrackTable::PrimaryKey = &AlbumTrack::m_id;

AlbumTrack::AlbumTrack(MediaExplorerPtr ml, sqlite::Row& row)
  : m_ml(ml) {
  row >> m_id
    >> m_mediaId
    >> m_artistId
    >> m_genreId
    >> m_trackNumber
    >> m_albumId
    >> m_discNumber
    >> m_isPresent;
}

AlbumTrack::AlbumTrack(MediaExplorerPtr ml, int64_t mediaId, unsigned int trackNumber, int64_t albumId, unsigned int discNumber)
  : m_ml(ml)
  , m_id(0)
  , m_mediaId(mediaId)
  , m_artistId(0)
  , m_genreId(0)
  , m_trackNumber(trackNumber)
  , m_albumId(albumId)
  , m_discNumber(discNumber)
  , m_isPresent(true) {
}

int64_t AlbumTrack::Id() const {
  return m_id;
}

ArtistPtr AlbumTrack::GetArtist() const {
  if(m_artistId == 0)
    return nullptr;
  auto lock = m_artist.Lock();
  if(m_artist.IsCached() == false) {
    m_artist = Artist::Fetch(m_ml, m_artistId);
  }
  return m_artist.Get();
}

bool AlbumTrack::setArtist(std::shared_ptr<Artist> artist) {
  static const std::string req = "UPDATE " + policy::AlbumTrackTable::Name +
    " SET artist_id = ? WHERE id_track = ?";
  if(artist->Id() == m_artistId)
    return true;
  if(sqlite::Tools::ExecuteUpdate(m_ml->GetConnection(), req, artist->Id(), m_id) == false)
    return false;
  m_artistId = artist->Id();
  m_artist = artist;
  return true;
}

bool AlbumTrack::createTable(DBConnection dbConnection) {
  static const std::string req = "CREATE TABLE IF NOT EXISTS " + policy::AlbumTrackTable::Name + "("
    "id_track INTEGER PRIMARY KEY AUTOINCREMENT,"
    "media_id INTEGER,"
    "artist_id UNSIGNED INTEGER,"
    "genre_id INTEGER,"
    "track_number UNSIGNED INTEGER,"
    "album_id UNSIGNED INTEGER NOT NULL,"
    "disc_number UNSIGNED INTEGER,"
    "is_present BOOLEAN NOT NULL DEFAULT 1,"
    "FOREIGN KEY (media_id) REFERENCES " + policy::MediaTable::Name + "(id_media)"
    " ON DELETE CASCADE,"
    "FOREIGN KEY (artist_id) REFERENCES " + policy::ArtistTable::Name + "(id_artist)"
    " ON DELETE CASCADE,"
    "FOREIGN KEY (genre_id) REFERENCES " + policy::GenreTable::Name + "(id_genre),"
    "FOREIGN KEY (album_id) REFERENCES Album(id_album) "
    " ON DELETE CASCADE"
    ")";
  static const std::string triggerReq = "CREATE TRIGGER IF NOT EXISTS is_track_present AFTER UPDATE OF is_present "
    "ON " + policy::MediaTable::Name +
    " BEGIN"
    " UPDATE " + policy::AlbumTrackTable::Name + " SET is_present = new.is_present WHERE media_id = new.id_media;"
    " END";

  return sqlite::Tools::ExecuteRequest(dbConnection, req) &&
    sqlite::Tools::ExecuteRequest(dbConnection, triggerReq);
}

std::shared_ptr<AlbumTrack> AlbumTrack::create(MediaExplorerPtr ml, int64_t albumId,
  std::shared_ptr<Media> media, unsigned int trackNb,
  unsigned int discNumber) {
  auto self = std::make_shared<AlbumTrack>(ml, media->Id(), trackNb, albumId, discNumber);
  static const std::string req = "INSERT INTO " + policy::AlbumTrackTable::Name
    + "(media_id, track_number, album_id, disc_number) VALUES(?, ?, ?, ?)";
  if(insert(ml, self, req, media->Id(), trackNb, albumId, discNumber) == false)
    return nullptr;
  self->m_media = media;
  return self;
}

AlbumTrackPtr AlbumTrack::fromMedia(MediaExplorerPtr ml, int64_t mediaId) {
  static const std::string req = "SELECT * FROM " + policy::AlbumTrackTable::Name +
    " WHERE media_id = ?";
  return Fetch(ml, req, mediaId);
}

std::vector<MediaPtr> AlbumTrack::fromGenre(MediaExplorerPtr ml, int64_t genreId, SortingCriteria sort, bool desc) {
  std::string req = "SELECT m.* FROM " + policy::MediaTable::Name + " m"
    " INNER JOIN " + policy::AlbumTrackTable::Name + " t ON m.id_media = t.media_id"
    " WHERE t.genre_id = ? ORDER BY ";
  switch(sort) {
  case SortingCriteria::Duration:
    req += "m.duration";
    break;
  case SortingCriteria::InsertionDate:
    req += "m.insertion_date";
    break;
  case SortingCriteria::ReleaseDate:
    req += "m.release_date";
    break;
  case SortingCriteria::Alpha:
    req += "m.title";
    break;
  default:
    if(desc == true)
      req += "t.artist_id DESC, t.album_id DESC, t.disc_number DESC, t.track_number DESC, m.filename";
    else
      req += "t.artist_id, t.album_id, t.disc_number, t.track_number, m.filename";
    break;
  }

  if(desc == true)
    req += " DESC";
  return Media::FetchAll<IMedia>(ml, req, genreId);
}

GenrePtr AlbumTrack::GetGenre() {
  auto l = m_genre.Lock();
  if(m_genre.IsCached() == false) {
    m_genre = Genre::Fetch(m_ml, m_genreId);
  }
  return m_genre.Get();
}

bool AlbumTrack::setGenre(std::shared_ptr<Genre> genre) {
  static const std::string req = "UPDATE " + policy::AlbumTrackTable::Name
    + " SET genre_id = ? WHERE id_track = ?";
  if(sqlite::Tools::ExecuteUpdate(m_ml->GetConnection(), req,
    sqlite::ForeignKey(genre != nullptr ? genre->Id() : 0),
    m_id) == false)
    return false;
  auto l = m_genre.Lock();
  m_genre = genre;
  m_genreId = genre != nullptr ? genre->Id() : 0;
  return true;
}

unsigned int AlbumTrack::TrackNumber() {
  return m_trackNumber;
}

unsigned int AlbumTrack::DiscNumber() const {
  return m_discNumber;
}

std::shared_ptr<IAlbum> AlbumTrack::GetAlbum() {
  // "Fail" early in case there's no album to fetch
  if(m_albumId == 0)
    return nullptr;

  auto lock = m_album.Lock();
  if(m_album.IsCached() == false)
    m_album = Album::Fetch(m_ml, m_albumId);
  return m_album.Get().lock();
}

std::shared_ptr<IMedia> AlbumTrack::GetMedia() {
  auto lock = m_media.Lock();
  if(m_media.IsCached() == false) {
    m_media = Media::Fetch(m_ml, m_mediaId);
  }
  return m_media.Get().lock();
}

}
