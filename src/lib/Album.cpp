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
#include "Media.h"
#include "database/SqliteTools.h"
#include "mediaexplorer/IGenre.h"

using mxp::policy::AlbumTable;
const std::string mxp::policy::AlbumTable::Name = "Album";
const std::string mxp::policy::AlbumTable::PrimaryKeyColumn = "id_album";
int64_t mxp::Album::* const mxp::policy::AlbumTable::PrimaryKey = &mxp::Album::m_id;

mxp::Album::Album(MediaExplorerPtr ml, sqlite::Row& row)
  : m_ml(ml)
  , m_id{}
  , m_artistId{}
  , m_releaseYear{}
  , m_nbTracks{}
  , m_duration{}
  , m_isPresent{} {
  row >> m_id
      >> m_title
      >> m_artistId
      >> m_releaseYear
      >> m_shortSummary
      >> m_artworkMrl
      >> m_nbTracks
      >> m_duration
      >> m_isPresent;
}

mxp::Album::Album(MediaExplorerPtr ml, const std::string& title)
  : m_ml(ml)
  , m_id(0)
  , m_title(title)
  , m_artistId(0)
  , m_releaseYear(~0u)
  , m_nbTracks(0)
  , m_duration(0)
  , m_isPresent(true) {
}

mxp::Album::Album(MediaExplorerPtr ml, const Artist* artist)
  : m_ml(ml)
  , m_id(0)
  , m_artistId(artist->Id())
  , m_releaseYear(~0u)
  , m_nbTracks(0)
  , m_duration(0)
  , m_isPresent(true) {
}

int64_t mxp::Album::Id() const {
  return m_id;
}

const std::string& mxp::Album::GetTitle() const {
  return m_title;
}

unsigned int mxp::Album::ReleaseYear() const {
  if(m_releaseYear == ~0U)
    return 0;
  return m_releaseYear;
}

bool mxp::Album::SetReleaseYear(unsigned int date, bool force) {
  if(date == m_releaseYear)
    return true;

  if(force == false) {
    if(m_releaseYear != ~0u && date != m_releaseYear) {
      // If we already have set the date back to 0, don't do it again.
      if(m_releaseYear == 0)
        return true;
      date = 0;
    }
  }

  static const auto req = "UPDATE " + AlbumTable::Name + " SET release_year=? WHERE " + AlbumTable::PrimaryKeyColumn + "=?";
  if(sqlite::Tools::ExecuteUpdate(m_ml->GetConnection(), req, date, m_id) == false)
    return false;
  m_releaseYear = date;
  return true;
}

const std::string& mxp::Album::ShortSummary() const {
  return m_shortSummary;
}

bool mxp::Album::SetShortSummary(const std::string& summary) {
  static const auto req = "UPDATE " + AlbumTable::Name + " SET short_summary=? WHERE " + AlbumTable::PrimaryKeyColumn + "=?";
  if(sqlite::Tools::ExecuteUpdate(m_ml->GetConnection(), req, summary, m_id) == false)
    return false;
  m_shortSummary = summary;
  return true;
}

const std::string& mxp::Album::GetArtworkMrl() const {
  return m_artworkMrl;
}

bool mxp::Album::SetArtworkMrl(const std::string& artworkMrl) {
  static const auto req = "UPDATE " + AlbumTable::Name + " SET artwork_mrl=? WHERE " + AlbumTable::PrimaryKeyColumn + "=?";
  if(sqlite::Tools::ExecuteUpdate(m_ml->GetConnection(), req, artworkMrl, m_id) == false)
    return false;
  m_artworkMrl = artworkMrl;
  return true;
}

std::string mxp::Album::OrderTracksBy(SortingCriteria sort, bool desc) {
  std::string req = " ORDER BY ";
  switch(sort) {
  case SortingCriteria::Alpha:
    req += "med.title";
    break;
  case SortingCriteria::Duration:
    req += "med.duration";
    break;
  case SortingCriteria::ReleaseDate:
    req += "med.release_date";
    break;
  default:
    if(desc == true)
      req += "att.disc_number DESC, att.track_number DESC, med.filename";
    else
      req += "att.disc_number, att.track_number, med.filename";
    break;
  }

  if(desc == true)
    req += " DESC";
  return req;
}

std::string mxp::Album::OrderBy(SortingCriteria sort, bool desc) {
  std::string req = " ORDER BY ";
  switch(sort) {
  case SortingCriteria::ReleaseDate:
    req += desc ? "release_year DESC, title" : "release_year, title";
    break;
  case SortingCriteria::Duration:
    req += "duration";
    if(desc == true) {
      req += " DESC";
    }
    break;
  default:
    req += "title";
    if(desc == true) {
      req += " DESC";
    }
    break;
  }
  return req;
}

std::vector<mxp::MediaPtr> mxp::Album::Tracks(SortingCriteria sort, bool desc) const {
  // This doesn't return the cached version, because it would be fairly complicated, if not impossible or
  // counter productive, to maintain a cache that respects all orderings.
  static const auto req = "SELECT med.* FROM " + policy::MediaTable::Name + " med "
    " INNER JOIN " + policy::AlbumTrackTable::Name + " att ON att.media_id = med.id_media "
    " WHERE att.album_id = ? AND med.is_present = 1";

  return Media::FetchAll<IMedia>(m_ml, req + OrderTracksBy(sort, desc), m_id);
}

std::vector<mxp::MediaPtr> mxp::Album::Tracks(GenrePtr genre, SortingCriteria sort, bool desc) const {
  static const auto req = "SELECT med.* FROM " + policy::MediaTable::Name + " med "
    " INNER JOIN " + policy::AlbumTrackTable::Name + " att ON att.media_id = med.id_media "
    " WHERE att.album_id = ? AND med.is_present = 1"
    " AND genre_id = ?";
  if(genre == nullptr) {
    return{};
  }

  return Media::FetchAll<IMedia>(m_ml, req + OrderTracksBy(sort, desc), m_id, genre->Id());
}

std::vector<mxp::MediaPtr>mxp::Album::CachedTracks() const {
  auto lock = m_tracks.Lock();
  if(m_tracks.IsCached() == false)
    m_tracks = Tracks(SortingCriteria::Default, false);
  return m_tracks.Get();
}

std::shared_ptr<mxp::AlbumTrack> mxp::Album::AddTrack(std::shared_ptr<Media> media, unsigned int trackNb, unsigned int discNumber) {
  auto t = m_ml->GetConnection()->NewTransaction();

  auto track = AlbumTrack::Create(m_ml, m_id, media, trackNb, discNumber);
  if(track == nullptr)
    return nullptr;
  media->SetAlbumTrack(track);
  // Assume the media will be saved by the caller
  m_nbTracks++;
  m_duration += media->GetDuration();
  t->Commit();
  auto lock = m_tracks.Lock();
  // Don't assume we have always have a valid value in m_tracks.
  // While it's ok to assume that if we are currently parsing the album, we
  // have a valid cache tracks, this isn't true when restarting an interrupted parsing.
  // The nbTracks value will be correct however. If it's equal to one, it means we're inserting
  // the first track in this album
  if(m_tracks.IsCached() == false && m_nbTracks == 1)
    m_tracks.MarkCached();
  if(m_tracks.IsCached() == true)
    m_tracks.Get().push_back(media);
  return track;
}

unsigned int mxp::Album::GetNumTracks() const {
  return m_nbTracks;
}

unsigned int mxp::Album::Duration() const {
  return m_duration;
}

mxp::ArtistPtr mxp::Album::AlbumArtist() const {
  if(m_artistId == 0)
    return nullptr;
  auto lock = m_albumArtist.Lock();
  if(m_albumArtist.IsCached() == false)
    m_albumArtist = Artist::Fetch(m_ml, m_artistId);
  return m_albumArtist.Get();
}

bool mxp::Album::SetAlbumArtist(std::shared_ptr<Artist> artist) {
  if(m_artistId == artist->Id())
    return true;
  if(artist->Id() == 0)
    return false;
  static const auto req = "UPDATE " + AlbumTable::Name + " SET artist_id=? WHERE " + AlbumTable::PrimaryKeyColumn + "=?";
  if(sqlite::Tools::ExecuteUpdate(m_ml->GetConnection(), req, artist->Id(), m_id) == false)
    return false;
  if(m_artistId != 0) {
    if(m_albumArtist.IsCached() == false)
      AlbumArtist();
    m_albumArtist.Get()->updateNbAlbum(-1);
  }
  m_artistId = artist->Id();
  m_albumArtist = artist;
  artist->updateNbAlbum(1);
  static const auto ftsReq = "UPDATE " + AlbumTable::Name + "Fts SET "
    " artist = ? WHERE rowid = ?";
  sqlite::Tools::ExecuteUpdate(m_ml->GetConnection(), ftsReq, artist->GetName(), m_id);
  return true;
}

std::vector<mxp::ArtistPtr> mxp::Album::Artists(bool desc) const {
  auto req = "SELECT art.* FROM " + policy::ArtistTable::Name + " art "
    "INNER JOIN AlbumArtistRelation aar ON aar.artist_id = art.id_artist "
    "WHERE aar.album_id = ? ORDER BY art.name";
  if(desc == true)
    req += " DESC";
  return Artist::FetchAll<IArtist>(m_ml, req, m_id);
}

bool mxp::Album::AddArtist(std::shared_ptr<Artist> artist) {
  static const std::string req = "INSERT OR IGNORE INTO AlbumArtistRelation VALUES(?, ?)";
  if(m_id == 0 || artist->Id() == 0) {
    LOG_ERROR("Both artist & album need to be inserted in database before being linked together");
    return false;
  }
  return sqlite::Tools::ExecuteInsert(m_ml->GetConnection(), req, m_id, artist->Id()) != 0;
}

bool mxp::Album::RemoveArtist(Artist* artist) {
  static const std::string req = "DELETE FROM AlbumArtistRelation WHERE album_id=? AND id_artist=?";
  return sqlite::Tools::ExecuteDelete(m_ml->GetConnection(), req, m_id, artist->Id());
}

bool mxp::Album::CreateTable(DBConnection connection) {
  static const auto req = "CREATE TABLE IF NOT EXISTS " + policy::AlbumTable::Name + "(" +
     AlbumTable::PrimaryKeyColumn + " INTEGER PRIMARY KEY AUTOINCREMENT,"
    "title TEXT COLLATE NOCASE,"
    "artist_id UNSIGNED INTEGER,"
    "release_year UNSIGNED INTEGER,"
    "short_summary TEXT,"
    "artwork_mrl TEXT,"
    "nb_tracks UNSIGNED INTEGER DEFAULT 0,"
    "duration UNSIGNED INTEGER NOT NULL DEFAULT 0,"
    "is_present BOOLEAN NOT NULL DEFAULT 1,"
    "FOREIGN KEY( artist_id ) REFERENCES " + policy::ArtistTable::Name + "(id_artist) ON DELETE CASCADE"
    ")";
  static const auto reqRel = "CREATE TABLE IF NOT EXISTS AlbumArtistRelation("
    "album_id INTEGER,"
    "artist_id INTEGER,"
    "PRIMARY KEY (album_id, artist_id),"
    "FOREIGN KEY(album_id) REFERENCES " + AlbumTable::Name + "(" + AlbumTable::PrimaryKeyColumn + ") ON DELETE CASCADE,"
    "FOREIGN KEY(artist_id) REFERENCES " + policy::ArtistTable::Name + "(" + policy::ArtistTable::PrimaryKeyColumn + ") ON DELETE CASCADE"
    ")";
  static const auto vtableReq = "CREATE VIRTUAL TABLE IF NOT EXISTS "
    + AlbumTable::Name + "Fts USING FTS3("
    "title,"
    "artist"
    ")";
  return sqlite::Tools::ExecuteRequest(connection, req) &&
    sqlite::Tools::ExecuteRequest(connection, reqRel) &&
    sqlite::Tools::ExecuteRequest(connection, vtableReq);
}

bool mxp::Album::CreateTriggers(DBConnection connection) {
  static const auto triggerReq = "CREATE TRIGGER IF NOT EXISTS is_album_present AFTER UPDATE OF "
    "is_present ON " + policy::AlbumTrackTable::Name +
    " BEGIN "
    " UPDATE " + AlbumTable::Name + " SET is_present="
    "(SELECT COUNT(id_track) FROM " + policy::AlbumTrackTable::Name + " WHERE album_id=new.album_id AND is_present=1) "
    "WHERE " + AlbumTable::PrimaryKeyColumn + "=new.album_id;"
    " END";
  static const auto deleteTriggerReq = "CREATE TRIGGER IF NOT EXISTS delete_album_track AFTER DELETE ON "
    + policy::AlbumTrackTable::Name +
    " BEGIN "
    " UPDATE " + AlbumTable::Name + " SET nb_tracks = nb_tracks - 1 WHERE " + AlbumTable::PrimaryKeyColumn + " = old.album_id;"
    " DELETE FROM " + AlbumTable::Name +
    " WHERE " + AlbumTable::PrimaryKeyColumn + "=old.album_id AND nb_tracks = 0;"
    " END";
  static const auto updateAddTrackTriggerReq = "CREATE TRIGGER IF NOT EXISTS add_album_track"
    " AFTER INSERT ON " + policy::AlbumTrackTable::Name +
    " BEGIN"
    " UPDATE " + AlbumTable::Name +
    " SET duration = duration + (SELECT duration FROM " + policy::MediaTable::Name + " WHERE id_media=new.media_id),"
    " nb_tracks = nb_tracks + 1"
    " WHERE " + AlbumTable::PrimaryKeyColumn + " = new.album_id;"
    " END";
  static const auto vtriggerInsert = "CREATE TRIGGER IF NOT EXISTS insert_album_fts AFTER INSERT ON "
    + policy::AlbumTable::Name +
    // Skip unknown albums
    " WHEN new.title IS NOT NULL"
    " BEGIN"
    " INSERT INTO " + AlbumTable::Name + "Fts(rowid, title) VALUES(new." + policy::AlbumTable::PrimaryKeyColumn + ", new.title);"
    " END";
  static const auto vtriggerDelete = "CREATE TRIGGER IF NOT EXISTS delete_album_fts BEFORE DELETE ON " + 
    AlbumTable::Name +
    // Unknown album probably won't be deleted, but better safe than sorry
    " WHEN old.title IS NOT NULL"
    " BEGIN"
    " DELETE FROM " + AlbumTable::Name + "Fts WHERE rowid = old." + policy::AlbumTable::PrimaryKeyColumn + ";"
    " END";
  return sqlite::Tools::ExecuteRequest(connection, triggerReq) &&
    sqlite::Tools::ExecuteRequest(connection, deleteTriggerReq) &&
    sqlite::Tools::ExecuteRequest(connection, updateAddTrackTriggerReq) &&
    sqlite::Tools::ExecuteRequest(connection, vtriggerInsert) &&
    sqlite::Tools::ExecuteRequest(connection, vtriggerDelete);
}

std::shared_ptr<mxp::Album> mxp::Album::Create(MediaExplorerPtr ml, const std::string& title) {
  auto album = std::make_shared<Album>(ml, title);
  static const auto req = "INSERT INTO " + AlbumTable::Name + "(" + AlbumTable::PrimaryKeyColumn + ", title) VALUES(NULL, ?)";
  if(insert(ml, album, req, title) == false)
    return nullptr;
  return album;
}

std::shared_ptr<mxp::Album> mxp::Album::CreateUnknownAlbum(MediaExplorerPtr ml, const Artist* artist) {
  auto album = std::make_shared<Album>(ml, artist);
  static const auto req = "INSERT INTO " + AlbumTable::Name + "(" + AlbumTable::PrimaryKeyColumn + ", artist_id) VALUES(NULL, ?)";
  if(insert(ml, album, req, artist->Id()) == false)
    return nullptr;
  return album;
}

std::vector<mxp::AlbumPtr> mxp::Album::Search(MediaExplorerPtr ml, const std::string& pattern) {
  static const auto req = "SELECT * FROM " + AlbumTable::Name + " WHERE " + AlbumTable::PrimaryKeyColumn + " IN "
    "(SELECT rowid FROM " + AlbumTable::Name + "Fts WHERE " +
    AlbumTable::Name + "Fts MATCH ?)"
    "AND is_present = 1";
  return FetchAll<IAlbum>(ml, req, pattern + "*");
}

std::vector<mxp::AlbumPtr> mxp::Album::FromArtist(MediaExplorerPtr ml, int64_t artistId, SortingCriteria sort, bool desc) {
  auto req = "SELECT * FROM " + AlbumTable::Name + " alb WHERE artist_id = ? AND is_present=1 ORDER BY ";
  switch(sort) {
  case SortingCriteria::Alpha:
    req += "title";
    if(desc == true)
      req += " DESC";
    break;
  default:
    // When listing albums of an artist, default order is by descending year (with album title
    // discrimination in case 2+ albums went out the same year)
    // This leads to DESC being used for "non-desc" case
    if(desc == true)
      req += "release_year, title";
    else
      req += "release_year DESC, title";
    break;
  }

  return FetchAll<IAlbum>(ml, req, artistId);
}

std::vector<mxp::AlbumPtr> mxp::Album::FindByGenre(MediaExplorerPtr ml, int64_t genreId, SortingCriteria sort, bool desc) {
 static const auto req = "SELECT a.* FROM " + AlbumTable::Name + " a "
    "INNER JOIN " + policy::AlbumTrackTable::Name + " att ON att.album_id = a." + AlbumTable::PrimaryKeyColumn + " "
    "WHERE att.genre_id = ? GROUP BY att.album_id";
  return FetchAll<IAlbum>(ml, req + OrderBy(sort, desc), genreId);
}

std::vector<mxp::AlbumPtr> mxp::Album::ListAll(MediaExplorerPtr ml, SortingCriteria sort, bool desc) {
  static const auto req = "SELECT * FROM " + AlbumTable::Name + " WHERE is_present=1";
  return FetchAll<IAlbum>(ml, req + OrderBy(sort, desc));
}
