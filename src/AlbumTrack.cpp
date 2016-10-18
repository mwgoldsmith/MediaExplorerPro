/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#include "stdafx.h"
#if HAVE_CONFIG_H
# include "config.h"
#endif

#include "Album.h"
#include "AlbumTrack.h"
#include "Artist.h"
#include "Genre.h"
#include "Media.h"
#include "database/SqliteTools.h"

const std::string mxp::policy::AlbumTrackTable::Name = "AlbumTrack";
const std::string mxp::policy::AlbumTrackTable::PrimaryKeyColumn = "id_track";
int64_t mxp::AlbumTrack::* const mxp::policy::AlbumTrackTable::PrimaryKey = &mxp::AlbumTrack::m_id;

mxp::AlbumTrack::AlbumTrack(MediaExplorerPtr ml, sqlite::Row& row)
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

mxp::AlbumTrack::AlbumTrack(MediaExplorerPtr ml, int64_t mediaId, unsigned int trackNumber, int64_t albumId, unsigned int discNumber)
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

int64_t mxp::AlbumTrack::Id() const {
  return m_id;
}

mxp::ArtistPtr mxp::AlbumTrack::GetArtist() const {
  if(m_artistId == 0)
    return nullptr;
  auto lock = m_artist.Lock();
  if(m_artist.IsCached() == false) {
    m_artist = Artist::Fetch(m_ml, m_artistId);
  }
  return m_artist.Get();
}

bool mxp::AlbumTrack::SetArtist(std::shared_ptr<Artist> artist) {
  static const auto req = "UPDATE " + policy::AlbumTrackTable::Name +
    " SET artist_id = ? WHERE " + policy::AlbumTrackTable::PrimaryKeyColumn + " = ?";
  if(artist->Id() == m_artistId)
    return true;
  if(sqlite::Tools::ExecuteUpdate(m_ml->GetConnection(), req, artist->Id(), m_id) == false)
    return false;
  m_artistId = artist->Id();
  m_artist = artist;
  return true;
}

mxp::GenrePtr mxp::AlbumTrack::GetGenre() {
  auto l = m_genre.Lock();
  if(m_genre.IsCached() == false) {
    m_genre = Genre::Fetch(m_ml, m_genreId);
  }
  return m_genre.Get();
}

bool mxp::AlbumTrack::SetGenre(std::shared_ptr<Genre> genre) {
  static const auto req = "UPDATE " + policy::AlbumTrackTable::Name
    + " SET genre_id = ? WHERE " + policy::AlbumTrackTable::PrimaryKeyColumn + " = ?";
  if(sqlite::Tools::ExecuteUpdate(m_ml->GetConnection(), req,
    sqlite::ForeignKey(genre != nullptr ? static_cast<unsigned int>(genre->Id()) : 0),
    m_id) == false)
    return false;
  auto l = m_genre.Lock();
  m_genre = genre;
  m_genreId = genre != nullptr ? genre->Id() : 0;
  return true;
}

unsigned int mxp::AlbumTrack::GetTrackNumber() {
  return m_trackNumber;
}

unsigned int mxp::AlbumTrack::GetDiscNumber() const {
  return m_discNumber;
}

std::shared_ptr<mxp::IAlbum> mxp::AlbumTrack::GetAlbum() {
  // "Fail" early in case there's no album to fetch
  if(m_albumId == 0)
    return nullptr;

  auto lock = m_album.Lock();
  if(m_album.IsCached() == false)
    m_album = Album::Fetch(m_ml, m_albumId);
  return m_album.Get().lock();
}

std::shared_ptr<mxp::IMedia> mxp::AlbumTrack::GetMedia() {
  auto lock = m_media.Lock();
  if(m_media.IsCached() == false) {
    m_media = Media::Fetch(m_ml, m_mediaId);
  }
  return m_media.Get().lock();
}


bool mxp::AlbumTrack::CreateTable(DBConnection dbConnection) {
  static const auto req = "CREATE TABLE IF NOT EXISTS " + policy::AlbumTrackTable::Name + "(" +
    policy::AlbumTrackTable::PrimaryKeyColumn + " INTEGER PRIMARY KEY AUTOINCREMENT,"
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
  static const auto triggerReq = "CREATE TRIGGER IF NOT EXISTS is_track_present AFTER UPDATE OF is_present "
    "ON " + policy::MediaTable::Name +
    " BEGIN"
    " UPDATE " + policy::AlbumTrackTable::Name + " SET is_present = new.is_present WHERE media_id = new.id_media;"
    " END";

  return sqlite::Tools::ExecuteRequest(dbConnection, req) &&
    sqlite::Tools::ExecuteRequest(dbConnection, triggerReq);
}

std::shared_ptr<mxp::AlbumTrack> mxp::AlbumTrack::Create(MediaExplorerPtr ml, int64_t albumId,
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

mxp::AlbumTrackPtr mxp::AlbumTrack::FindByMedia(MediaExplorerPtr ml, int64_t mediaId) {
  static const auto req = "SELECT * FROM " + policy::AlbumTrackTable::Name +
    " WHERE media_id = ?";
  return Fetch(ml, req, mediaId);
}

std::vector<mxp::MediaPtr> mxp::AlbumTrack::FindByGenre(MediaExplorerPtr ml, int64_t genreId, SortingCriteria sort, bool desc) {
  auto req = "SELECT m.* FROM " + policy::MediaTable::Name + " m"
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
