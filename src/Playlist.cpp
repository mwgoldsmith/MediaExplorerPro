/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#include "stdafx.h"
#if HAVE_CONFIG_H
# include "config.h"
#endif

#include "Media.h"
#include "Playlist.h"
#include "database/SqliteTools.h"

using mxp::policy::PlaylistTable;
const mstring PlaylistTable::Name = "Playlist";
const mstring PlaylistTable::PrimaryKeyColumn = "id_playlist";
int64_t mxp::Playlist::* const PlaylistTable::PrimaryKey = &mxp::Playlist::m_id;

mxp::Playlist::Playlist(MediaExplorerPtr ml, sqlite::Row& row)
  : m_ml(ml) {
  row >> m_id
      >> m_name
      >> m_creationDate;
}

mxp::Playlist::Playlist(MediaExplorerPtr ml, const mstring& name)
  : m_ml(ml)
  , m_id(0)
  , m_name(name)
  , m_creationDate(time(nullptr)) {}

int64_t mxp::Playlist::Id() const {
  return m_id;
}

const mstring& mxp::Playlist::GetName() const {
  return m_name;
}

bool mxp::Playlist::SetName(const mstring& name) {
  if (name == m_name)
    return true;

  static const auto req = "UPDATE " + PlaylistTable::Name + " SET name = ? WHERE id_playlist = ?";
  if (sqlite::Tools::ExecuteUpdate(m_ml->GetConnection(), req, name, m_id) == false)
    return false;

  m_name = name;

  return true;
}

time_t mxp::Playlist::GetCreationDate() const {
  return m_creationDate;
}

std::vector<mxp::MediaPtr> mxp::Playlist::GetMedia() const {
  static const auto req = "SELECT m.* FROM " + policy::MediaTable::Name + " m "
    "LEFT JOIN PlaylistMediaRelation pmr ON pmr.media_id = m." + policy::MediaTable::PrimaryKeyColumn + " "
    "WHERE pmr.playlist_id = ? AND m.is_present = 1 "
    "ORDER BY pmr.position";
  return Media::FetchAll<IMedia>(m_ml, req, m_id);
}

bool mxp::Playlist::Append(int64_t mediaId) {
  return Add(mediaId, 0);
}

bool mxp::Playlist::Add(int64_t mediaId, unsigned int position) {
  static const mstring req = "INSERT INTO PlaylistMediaRelation(media_id, playlist_id, position) VALUES(?, ?, ?)";
  // position isn't a foreign key, but we want it to be passed as NULL if it equals to 0
  // When the position is NULL, the insertion triggers takes care of counting the number of records to auto append.
  try {
    return sqlite::Tools::ExecuteInsert(m_ml->GetConnection(), req, mediaId, m_id, sqlite::ForeignKey{position}) != 0;
  } catch (const sqlite::errors::ConstraintViolation& ex) {
    LOG_WARN("Rejected playlist insertion: ", ex.what());
    return false;
  }
}

bool mxp::Playlist::Move(int64_t mediaId, unsigned int position) {
  if (position == 0)
    return false;
  static const mstring req = "UPDATE PlaylistMediaRelation SET position = ? WHERE playlist_id = ? AND media_id = ?";
  return sqlite::Tools::ExecuteUpdate(m_ml->GetConnection(), req, position, m_id, mediaId);
}

bool mxp::Playlist::Remove(int64_t mediaId) {
  static const mstring req = "DELETE FROM PlaylistMediaRelation WHERE playlist_id = ? AND media_id = ?";
  return sqlite::Tools::ExecuteDelete(m_ml->GetConnection(), req, m_id, mediaId);
}

bool mxp::Playlist::CreateTable(DBConnection connection) {
  static const auto req = "CREATE TABLE IF NOT EXISTS " + PlaylistTable::Name + "(" + 
    PlaylistTable::PrimaryKeyColumn + " INTEGER PRIMARY KEY AUTOINCREMENT,"
    "name TEXT UNIQUE,"
    "creation_date UNSIGNED INT NOT NULL"
    ")";
  static const auto relTableReq = "CREATE TABLE IF NOT EXISTS PlaylistMediaRelation("
    "media_id INTEGER,"
    "playlist_id INTEGER,"
    "position INTEGER,"
    "PRIMARY KEY(media_id, playlist_id),"
    "FOREIGN KEY(media_id) REFERENCES " + policy::MediaTable::Name + "(" + policy::MediaTable::PrimaryKeyColumn + ") ON DELETE CASCADE, "
    "FOREIGN KEY(playlist_id) REFERENCES " + PlaylistTable::Name +  "(" + PlaylistTable::PrimaryKeyColumn + ") ON DELETE CASCADE, "
    "CONSTRAINT playlist_position_unique UNIQUE (playlist_id, position)"
    ")";
  static const auto vtableReq = "CREATE VIRTUAL TABLE IF NOT EXISTS " + PlaylistTable::Name + "Fts USING FTS3("
    "name"
    ")";
  return sqlite::Tools::ExecuteRequest(connection, req) &&
      sqlite::Tools::ExecuteRequest(connection, relTableReq) &&
      sqlite::Tools::ExecuteRequest(connection, vtableReq);
}

bool mxp::Playlist::CreateTriggers(DBConnection connection) {
  static const mstring req = "CREATE TRIGGER IF NOT EXISTS update_playlist_order AFTER UPDATE OF position"
      " ON PlaylistMediaRelation"
      " BEGIN "
      "UPDATE PlaylistMediaRelation SET position = position + 1"
      " WHERE playlist_id = new.playlist_id"
      " AND position = new.position"
      // We don't want to trigger a self-update when the insert trigger fires.
      " AND media_id != new.media_id;"
      " END";
  static const mstring autoAppendReq = "CREATE TRIGGER IF NOT EXISTS append_new_playlist_record AFTER INSERT"
      " ON PlaylistMediaRelation"
      " WHEN new.position IS NULL"
      " BEGIN "
      " UPDATE PlaylistMediaRelation SET position = ("
      "SELECT COUNT(media_id) FROM PlaylistMediaRelation WHERE playlist_id = new.playlist_id"
      ") WHERE playlist_id=new.playlist_id AND media_id = new.media_id;"
      " END";
  static const mstring autoShiftPosReq = "CREATE TRIGGER IF NOT EXISTS update_playlist_order_on_insert AFTER INSERT"
      " ON PlaylistMediaRelation"
      " WHEN new.position IS NOT NULL"
      " BEGIN "
      "UPDATE PlaylistMediaRelation SET position = position + 1"
      " WHERE playlist_id = new.playlist_id"
      " AND position = new.position"
      " AND media_id != new.media_id;"
      " END";
  static const auto vtriggerInsert = "CREATE TRIGGER IF NOT EXISTS insert_playlist_fts AFTER INSERT ON "
      + PlaylistTable::Name +
      " BEGIN"
      " INSERT INTO " + PlaylistTable::Name + "Fts(rowid, name) VALUES(new.id_playlist, new.name);"
      " END";
  static const auto vtriggerUpdate = "CREATE TRIGGER IF NOT EXISTS update_playlist_fts AFTER UPDATE OF name"
      " ON " + PlaylistTable::Name +
      " BEGIN"
      " UPDATE " + PlaylistTable::Name + "Fts SET name = new.name WHERE rowid = new.id_playlist;"
      " END";
  static const auto vtriggerDelete = "CREATE TRIGGER IF NOT EXISTS delete_playlist_fts BEFORE DELETE ON "
      + PlaylistTable::Name +
      " BEGIN"
      " DELETE FROM " + PlaylistTable::Name + "Fts WHERE rowid = old.id_playlist;"
      " END";

  return sqlite::Tools::ExecuteRequest(connection, req) &&
      sqlite::Tools::ExecuteRequest(connection, autoAppendReq) &&
      sqlite::Tools::ExecuteRequest(connection, autoShiftPosReq) &&
      sqlite::Tools::ExecuteRequest(connection, vtriggerInsert) &&
      sqlite::Tools::ExecuteRequest(connection, vtriggerUpdate) &&
      sqlite::Tools::ExecuteRequest(connection, vtriggerDelete);
}

std::shared_ptr<mxp::Playlist> mxp::Playlist::Create(MediaExplorerPtr ml, const mstring& name) noexcept {
  static const auto req = "INSERT INTO " + PlaylistTable::Name + "(name, creation_date) VALUES(?, ?)";
  std::shared_ptr<Playlist> self;

  try {
    self = std::make_shared<Playlist>(ml, name);
    if(insert(ml, self, req, name, self->m_creationDate) == false) {
      self = nullptr;
    }
  } catch(sqlite::errors::ConstraintViolation& ex) {
    LOG_WARN("Failed to create playlist (PL-CR-0100): ", ex.what());
    self = nullptr;
  } catch(std::exception& ex) {
    LOG_WARN("Failed to create playlist (PL-CR-0200): ", ex.what());
    self = nullptr;
  }

  return self;
}

std::vector<mxp::PlaylistPtr> mxp::Playlist::Search(MediaExplorerPtr ml, const mstring& name) {
  static const auto req = "SELECT * FROM " + PlaylistTable::Name + " WHERE id_playlist IN "
      "(SELECT rowid FROM " + PlaylistTable::Name + "Fts WHERE name MATCH ?)";
  return FetchAll<IPlaylist>(ml, req, name + "*");
}

std::vector<mxp::PlaylistPtr> mxp::Playlist::ListAll(MediaExplorerPtr ml, SortingCriteria sort, bool desc) {
  auto req = "SELECT * FROM " + PlaylistTable::Name + " ORDER BY ";
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
