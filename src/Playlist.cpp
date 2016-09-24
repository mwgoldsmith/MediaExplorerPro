﻿/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#if HAVE_CONFIG_H
# include "config.h"
#endif

#include "Playlist.h"
#include "Media.h"

namespace mxp {

namespace policy {
const std::string PlaylistTable::Name = "Playlist";
const std::string PlaylistTable::PrimaryKeyColumn = "id_playlist";
int64_t Playlist::* const PlaylistTable::PrimaryKey = &Playlist::m_id;
}

Playlist::Playlist(MediaExplorerPtr ml, sqlite::Row& row)
  : m_ml(ml) {
  row >> m_id
      >> m_name
      >> m_creationDate;
}

Playlist::Playlist(MediaExplorerPtr ml, const std::string& name)
  : m_ml(ml)
  , m_id(0)
  , m_name(name)
  , m_creationDate(time(nullptr)) {}

std::shared_ptr<Playlist> Playlist::create(MediaExplorerPtr ml, const std::string& name) {
  auto self = std::make_shared<Playlist>(ml, name);
  static const std::string req = "INSERT INTO " + policy::PlaylistTable::Name +
      "(name, creation_date) VALUES(?, ?)";

  try {
    if (insert(ml, self, req, name, self->m_creationDate) == false) {
      return nullptr;
    }

    return self;
  } catch (sqlite::errors::ConstraintViolation& ex) {
    LOG_WARN( "Failed to create playlist: ", ex.what() );
  }

  return nullptr;
}

int64_t Playlist::id() const {
  return m_id;
}

const std::string& Playlist::name() const {
  return m_name;
}

bool Playlist::setName(const std::string& name) {
  if (name == m_name)
    return true;
  static const std::string req = "UPDATE " + policy::PlaylistTable::Name + " SET name = ? WHERE id_playlist = ?";
  if (sqlite::Tools::executeUpdate(m_ml->GetConnection(), req, name, m_id) == false)
    return false;
  m_name = name;
  return true;
}

unsigned int Playlist::creationDate() const {
  return m_creationDate;
}

std::vector<MediaPtr> Playlist::media() const {
  static const std::string req = "SELECT m.* FROM " + policy::MediaTable::Name + " m "
      "LEFT JOIN PlaylistMediaRelation pmr ON pmr.media_id = m.id_media "
      "WHERE pmr.playlist_id = ? AND m.is_present = 1 "
      "ORDER BY pmr.position";
  return Media::FetchAll<IMedia>(m_ml, req, m_id);
}

bool Playlist::append(int64_t mediaId) {
  return add(mediaId, 0);
}

bool Playlist::add(int64_t mediaId, unsigned int position) {
  static const std::string req = "INSERT INTO PlaylistMediaRelation(media_id, playlist_id, position) VALUES(?, ?, ?)";
  // position isn't a foreign key, but we want it to be passed as NULL if it equals to 0
  // When the position is NULL, the insertion triggers takes care of counting the number of records to auto append.
  try {
    return sqlite::Tools::executeInsert(m_ml->GetConnection(), req, mediaId, m_id, sqlite::ForeignKey{position});
  } catch (const sqlite::errors::ConstraintViolation& ex) {
    LOG_WARN("Rejected playlist insertion: ", ex.what());
    return false;
  }
}

bool Playlist::move(int64_t mediaId, unsigned int position) {
  if (position == 0)
    return false;
  static const std::string req = "UPDATE PlaylistMediaRelation SET position = ? WHERE "
      "playlist_id = ? AND media_id = ?";
  return sqlite::Tools::executeUpdate(m_ml->GetConnection(), req, position, m_id, mediaId);
}

bool Playlist::remove(int64_t mediaId) {
  static const std::string req = "DELETE FROM PlaylistMediaRelation WHERE playlist_id = ? AND media_id = ?";
  return sqlite::Tools::executeDelete(m_ml->GetConnection(), req, m_id, mediaId);
}

bool Playlist::createTable(DBConnection dbConn) {
  static const std::string req = "CREATE TABLE IF NOT EXISTS " + policy::PlaylistTable::Name + 
      "(" + policy::PlaylistTable::PrimaryKeyColumn + " INTEGER PRIMARY KEY AUTOINCREMENT,"
      "name TEXT UNIQUE,"
      "creation_date UNSIGNED INT NOT NULL"
      ")";
  static const std::string relTableReq = "CREATE TABLE IF NOT EXISTS PlaylistMediaRelation("
      "media_id INTEGER,"
      "playlist_id INTEGER,"
      "position INTEGER,"
      "PRIMARY KEY(media_id, playlist_id),"
      "FOREIGN KEY(media_id) REFERENCES " + policy::MediaTable::Name +
      "(" + policy::MediaTable::PrimaryKeyColumn + ") ON DELETE CASCADE,"
      "FOREIGN KEY(playlist_id) REFERENCES " + policy::PlaylistTable::Name + 
      "(" + policy::PlaylistTable::PrimaryKeyColumn + ") ON DELETE CASCADE"
      ")";
  static const std::string vtableReq = "CREATE VIRTUAL TABLE IF NOT EXISTS "
      + policy::PlaylistTable::Name + "Fts USING FTS3("
        "name"
        ")";
  //FIXME Enforce (playlist_id,position) uniqueness
  return sqlite::Tools::executeRequest(dbConn, req) &&
      sqlite::Tools::executeRequest(dbConn, relTableReq) &&
      sqlite::Tools::executeRequest(dbConn, vtableReq);
}

bool Playlist::createTriggers(DBConnection dbConn) {
  static const std::string req = "CREATE TRIGGER IF NOT EXISTS update_playlist_order AFTER UPDATE OF position"
      " ON PlaylistMediaRelation"
      " BEGIN "
      "UPDATE PlaylistMediaRelation SET position = position + 1"
      " WHERE playlist_id = new.playlist_id"
      " AND position = new.position"
      // We don't to trigger a self-update when the insert trigger fires.
      " AND media_id != new.media_id;"
      " END";
  static const std::string autoAppendReq = "CREATE TRIGGER IF NOT EXISTS append_new_playlist_record AFTER INSERT"
      " ON PlaylistMediaRelation"
      " WHEN new.position IS NULL"
      " BEGIN "
      " UPDATE PlaylistMediaRelation SET position = ("
      "SELECT COUNT(media_id) FROM PlaylistMediaRelation WHERE playlist_id = new.playlist_id"
      ") WHERE playlist_id=new.playlist_id AND media_id = new.media_id;"
      " END";
  static const std::string autoShiftPosReq = "CREATE TRIGGER IF NOT EXISTS update_playlist_order_on_insert AFTER INSERT"
      " ON PlaylistMediaRelation"
      " WHEN new.position IS NOT NULL"
      " BEGIN "
      "UPDATE PlaylistMediaRelation SET position = position + 1"
      " WHERE playlist_id = new.playlist_id"
      " AND position = new.position"
      " AND media_id != new.media_id;"
      " END";
  static const std::string vtriggerInsert = "CREATE TRIGGER IF NOT EXISTS insert_playlist_fts AFTER INSERT ON "
      + policy::PlaylistTable::Name +
      " BEGIN"
      " INSERT INTO " + policy::PlaylistTable::Name + "Fts(rowid, name) VALUES(new.id_playlist, new.name);"
      " END";
  static const std::string vtriggerUpdate = "CREATE TRIGGER IF NOT EXISTS update_playlist_fts AFTER UPDATE OF name"
      " ON " + policy::PlaylistTable::Name +
      " BEGIN"
      " UPDATE " + policy::PlaylistTable::Name + "Fts SET name = new.name WHERE rowid = new.id_playlist;"
      " END";
  static const std::string vtriggerDelete = "CREATE TRIGGER IF NOT EXISTS delete_playlist_fts BEFORE DELETE ON "
      + policy::PlaylistTable::Name +
      " BEGIN"
      " DELETE FROM " + policy::PlaylistTable::Name + "Fts WHERE rowid = old.id_playlist;"
      " END";
  return sqlite::Tools::executeRequest(dbConn, req) &&
      sqlite::Tools::executeRequest(dbConn, autoAppendReq) &&
      sqlite::Tools::executeRequest(dbConn, autoShiftPosReq) &&
      sqlite::Tools::executeRequest(dbConn, vtriggerInsert) &&
      sqlite::Tools::executeRequest(dbConn, vtriggerUpdate) &&
      sqlite::Tools::executeRequest(dbConn, vtriggerDelete);
}

std::vector<PlaylistPtr> Playlist::search(MediaExplorerPtr ml, const std::string& name) {
  static const std::string req = "SELECT * FROM " + policy::PlaylistTable::Name + " WHERE id_playlist IN "
      "(SELECT rowid FROM " + policy::PlaylistTable::Name + "Fts WHERE name MATCH ?)";
  return FetchAll<IPlaylist>(ml, req, name + "*");
}

std::vector<PlaylistPtr> Playlist::listAll(MediaExplorerPtr ml, SortingCriteria sort, bool desc) {
  std::string req = "SELECT * FROM " + policy::PlaylistTable::Name + " ORDER BY ";
  switch (sort) {
  case SortingCriteria::InsertionDate:
    req += "creation_date";
    break;
  default:
    req += "name";
    break;
  }
  if (desc == true)
    req += " DESC";
  return FetchAll<IPlaylist>(ml, req);
}

}
