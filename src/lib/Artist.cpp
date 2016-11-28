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

namespace mxp {

using mxp::policy::ArtistTable;
const std::string policy::ArtistTable::Name = "Artist";
const std::string policy::ArtistTable::PrimaryKeyColumn = "id_artist";
int64_t Artist::*const policy::ArtistTable::PrimaryKey = &Artist::m_id;

Artist::Artist(MediaExplorerPtr ml, sqlite::Row& row)
  : m_ml(ml)
  , m_id{}
  , m_nbAlbums{}
  , m_isPresent{} {
  row >> m_id
      >> m_name
      >> m_shortBio
      >> m_artworkMrl
      >> m_nbAlbums
      >> m_mbId
      >> m_isPresent;
}

Artist::Artist(MediaExplorerPtr ml, const std::string& name)
  : m_ml(ml)
  , m_id(0)
  , m_name(name)
  , m_nbAlbums(0)
  , m_isPresent(true) {
}

int64_t Artist::Id() const {
  return m_id;
}

const std::string &Artist::GetName() const {
  return m_name;
}

const std::string &Artist::GetShortBio() const {
  return m_shortBio;
}

bool Artist::SetShortBio(const std::string &shortBio) {
  static const auto req = "UPDATE " + ArtistTable::Name + " SET shortbio = ? WHERE " + ArtistTable::PrimaryKeyColumn + " = ?";
  if(sqlite::Tools::ExecuteUpdate(m_ml->GetConnection(), req, shortBio, m_id) == false)
    return false;
  m_shortBio = shortBio;
  return true;
}

std::vector<AlbumPtr> Artist::albums(SortingCriteria sort, bool desc) const {
  return Album::FromArtist(m_ml, m_id, sort, desc);
}

std::vector<MediaPtr> Artist::media(SortingCriteria sort, bool desc) const {
  auto req = "SELECT med.* FROM " + policy::MediaTable::Name + " med "
    "INNER JOIN MediaArtistRelation mar ON mar.media_id = med.id_media "
    "WHERE mar.artist_id = ? AND med.is_present = 1 ORDER BY ";
  switch(sort) {
  case SortingCriteria::Duration:
    req += "med.duration";
    break;
  case SortingCriteria::InsertionDate:
    req += "med.insertion_date";
    break;
  case SortingCriteria::ReleaseDate:
    req += "med.release_date";
    break;
  default:
    req += "med.title";
    break;
  }

  if(desc == true)
    req += " DESC";
  return Media::FetchAll<IMedia>(m_ml, req, m_id);
}

bool Artist::AddMedia(Media& media) {
  static const std::string req = "INSERT INTO MediaArtistRelation VALUES(?, ?)";
  // If track's ID is 0, the request will fail due to table constraints
  sqlite::ForeignKey artistForeignKey(m_id);
  return sqlite::Tools::ExecuteInsert(m_ml->GetConnection(), req, media.Id(), artistForeignKey) != 0;
}

const std::string& Artist::GetArtworkMrl() const {
  return m_artworkMrl;
}

bool Artist::SetArtworkMrl(const std::string& artworkMrl) {
  if(m_artworkMrl == artworkMrl)
    return true;
  static const auto req = "UPDATE " + ArtistTable::Name + " SET artwork_mrl=? WHERE " + ArtistTable::PrimaryKeyColumn + "=?";
  if(sqlite::Tools::ExecuteUpdate(m_ml->GetConnection(), req, artworkMrl, m_id) == false)
    return false;
  m_artworkMrl = artworkMrl;
  return true;
}

bool Artist::updateNbAlbum(int increment) {
  assert(increment != 0);
  assert(increment > 0 || (increment < 0 && m_nbAlbums >= 1));

  static const auto req = "UPDATE " + ArtistTable::Name + " SET nb_albums = nb_albums + ? WHERE " + ArtistTable::PrimaryKeyColumn + "=?";
  if(sqlite::Tools::ExecuteUpdate(m_ml->GetConnection(), req, increment, m_id) == false)
    return false;
  m_nbAlbums += increment;
  return true;
}

std::shared_ptr<Album> Artist::unknownAlbum() {
  static const auto req = "SELECT * FROM " + policy::AlbumTable::Name + " WHERE artist_id=? AND title IS NULL";
  auto album = Album::Fetch(m_ml, req, m_id);
  if(album == nullptr) {
    album = Album::CreateUnknownAlbum(m_ml, this);
    if(album == nullptr)
      return nullptr;
    if(updateNbAlbum(1) == false) {
      Album::destroy(m_ml, album->Id());
      return nullptr;
    }
  }
  return album;
}

const std::string& Artist::musicBrainzId() const {
  return m_mbId;
}

bool Artist::setMusicBrainzId(const std::string& mbId) {
  static const auto req = "UPDATE " + ArtistTable::Name + " SET mb_id=? WHERE " + ArtistTable::PrimaryKeyColumn + "=?";
  if(mbId == m_mbId)
    return true;
  if(sqlite::Tools::ExecuteUpdate(m_ml->GetConnection(), req, mbId, m_id) == false)
    return false;
  m_mbId = mbId;
  return true;
}

bool Artist::CreateTable(DBConnection connection) noexcept {
  static const auto req = "CREATE TABLE IF NOT EXISTS " +
    ArtistTable::Name +
    "("
    "id_artist INTEGER PRIMARY KEY AUTOINCREMENT,"
    "name TEXT COLLATE NOCASE UNIQUE ON CONFLICT FAIL,"
    "shortbio TEXT,"
    "artwork_mrl TEXT,"
    "nb_albums UNSIGNED INT DEFAULT 0,"
    "mb_id TEXT,"
    "is_present BOOLEAN NOT NULL DEFAULT 1"
    ")";
  static const auto reqRel = "CREATE TABLE IF NOT EXISTS MediaArtistRelation("
    "media_id INTEGER NOT NULL,"
    "artist_id INTEGER,"
    "PRIMARY KEY (media_id, artist_id),"
    "FOREIGN KEY(media_id) REFERENCES " + policy::MediaTable::Name + "(" + policy::MediaTable::PrimaryKeyColumn + ") ON DELETE CASCADE,"
    "FOREIGN KEY(artist_id) REFERENCES " + ArtistTable::Name + "("+ ArtistTable::PrimaryKeyColumn + ") ON DELETE CASCADE"
    ")";
#ifdef USE_FTS
  static const auto reqFts = "CREATE VIRTUAL TABLE IF NOT EXISTS " +
    policy::ArtistTable::Name + "Fts USING FTS3("
    "name"
    ")";
#endif

  bool success;
  try {
    success = sqlite::Tools::ExecuteRequest(connection, req) &&
      sqlite::Tools::ExecuteRequest(connection, reqRel);
#ifdef USE_FTS
    if(success == true) {
      success = sqlite::Tools::ExecuteRequest(connection, reqFts);
    }
#endif
  } catch(std::exception& ex) {
    LOG_ERROR("Failed to create table for Artist: ", ex.what());
    success = false;
  }

  return success;
}

bool Artist::CreateTriggers(DBConnection connection) noexcept {
  static const auto triggerReq = "CREATE TRIGGER IF NOT EXISTS has_album_present AFTER UPDATE OF "
    "is_present ON " + policy::AlbumTable::Name +
    " BEGIN "
    " UPDATE " + ArtistTable::Name + " SET is_present="
    "(SELECT COUNT(id_album) FROM " + policy::AlbumTable::Name + " WHERE artist_id=new.artist_id AND is_present=1) "
    "WHERE id_artist=new.artist_id;"
    " END";
#ifdef USE_FTS
  static const auto ftsInsertTrigger = "CREATE TRIGGER IF NOT EXISTS insert_artist_fts"
    " AFTER INSERT ON " + ArtistTable::Name +
    " BEGIN"
    " INSERT INTO " + ArtistTable::Name + "Fts(rowid,name) VALUES(new.id_artist, new.name);"
    " END";
  static const auto ftsDeleteTrigger = "CREATE TRIGGER IF NOT EXISTS delete_artist_fts"
    " BEFORE DELETE ON " + ArtistTable::Name +
    " BEGIN"
    " DELETE FROM " + ArtistTable::Name + "Fts WHERE rowid=old.id_artist;"
    " END";
#endif

  bool success;
  try {
    success = sqlite::Tools::ExecuteRequest(connection, triggerReq);
#ifdef USE_FTS
    if(success == true) {
      success = sqlite::Tools::ExecuteRequest(connection, ftsInsertTrigger) &&
        sqlite::Tools::ExecuteRequest(connection, ftsDeleteTrigger);
    }
#endif
  } catch (std::exception& ex) {
    LOG_ERROR("Failed to create triggers for Artist: ", ex.what());
    success = false;
  }

  return success;
}

bool Artist::createDefaultArtists(DBConnection dbConnection) {
  // Don't rely on Artist::create, since we want to insert or do nothing here.
  // This will skip the cache for those new entities, but they will be inserted soon enough anyway.
  static const auto req = "INSERT OR IGNORE INTO " + ArtistTable::Name +
    "(id_artist) VALUES(?),(?)";
  sqlite::Tools::ExecuteInsert(dbConnection, req, UnknownArtistID, VariousArtistID);
  // Always return true. The insertion might succeed, but we consider it a failure when 0 row
  // gets inserted, while we are explicitely specifying "OR IGNORE" here.
  return true;
}

std::shared_ptr<Artist> Artist::Create(MediaExplorerPtr ml, const std::string &name) {
  auto artist = std::make_shared<Artist>(ml, name);
  static const auto req = "INSERT INTO " + ArtistTable::Name + "(id_artist, name) VALUES(NULL, ?)";
  if(insert(ml, artist, req, name) == false)
    return nullptr;
  return artist;
}

std::vector<ArtistPtr> Artist::Search(MediaExplorerPtr ml, const std::string& name) {
  static const auto req = "SELECT * FROM " + ArtistTable::Name + " WHERE "
#ifdef USE_FTS
    "id_artist IN (SELECT rowid FROM " + ArtistTable::Name + "Fts WHERE name MATCH ?)"
#else
    "name LIKE '%?%' "
#endif
    "AND is_present != 0";
  auto value = name;
#ifdef USE_FTS
  value = value + "*";
#endif
  return FetchAll<IArtist>(ml, req, value);
}

std::vector<ArtistPtr> Artist::ListAll(MediaExplorerPtr ml, SortingCriteria sort, bool desc) {
  auto req = "SELECT * FROM " + ArtistTable::Name + " WHERE nb_albums > 0 AND is_present = 1 ORDER BY ";
    req += "name";
  if(desc == true)
    req += " DESC";
  return FetchAll<IArtist>(ml, req);
}

}
