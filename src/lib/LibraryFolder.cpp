/*****************************************************************************
* Media Explorer
*****************************************************************************/

#include "stdafx.h"
#if HAVE_CONFIG_H
# include "config.h"
#endif

#include "LibraryFolder.h"
#include "Media.h"
#include "database/SqliteTools.h"

using mxp::policy::LibraryFolderTable;
const std::string LibraryFolderTable::Name = "LibraryFolder";
const std::string LibraryFolderTable::PrimaryKeyColumn = "id_library_folder";
int64_t mxp::LibraryFolder::* const LibraryFolderTable::PrimaryKey = &mxp::LibraryFolder::m_id;

const std::string mxp::LibraryFolder::s_all    = "All";
const std::string mxp::LibraryFolder::s_video  = "Video";
const std::string mxp::LibraryFolder::s_audio  = "Audio";
const std::string mxp::LibraryFolder::s_movies = "Movies";
const std::string mxp::LibraryFolder::s_shows  = "Shows";
const std::string mxp::LibraryFolder::s_albums = "Albums";

mxp::LibraryFolder::LibraryFolder(MediaExplorerPtr ml, sqlite::Row& row)
  : m_ml(ml)
  , m_id{}
  , m_isVirtual{}
  , m_isHidden{}
  , m_position{}
  , m_parentId{} {
  row >> m_id
      >> m_name
      >> m_isVirtual
      >> m_isHidden
      >> m_position
      >> m_parentId;
}

mxp::LibraryFolder::LibraryFolder(MediaExplorerPtr ml, const std::string& name, int64_t parentId, unsigned int position, bool isVirtual, bool isHidden)
  : m_ml(ml)
  , m_id(0)
  , m_name(name)
  , m_isVirtual(isVirtual)
  , m_isHidden(isHidden)
  , m_position(position)
  , m_parentId(parentId) {}

int64_t mxp::LibraryFolder::Id() const {
  return m_id;
}

const std::string& mxp::LibraryFolder::GetName() const {
  return m_name;
}

bool mxp::LibraryFolder::SetName(const std::string& name) {
  if(m_name == name)
    return true;
  if(m_isVirtual)
    return false;

  static const auto req = "UPDATE " + LibraryFolderTable::Name + " SET name=? WHERE " + LibraryFolderTable::PrimaryKeyColumn + "=?";
  if(sqlite::Tools::ExecuteUpdate(m_ml->GetConnection(), req, name, m_id) == false)
    return false;

  m_name = name;

  return true;
}

bool mxp::LibraryFolder::IsVirtual() const {
  return m_isVirtual;
}

bool mxp::LibraryFolder::IsHidden() const {
  return m_isHidden;
}

bool mxp::LibraryFolder::SetHidden(bool value) {
  if(m_isHidden == value)
    return true;
  static const auto req = "UPDATE " + LibraryFolderTable::Name + " SET is_hidden=? WHERE " + LibraryFolderTable::PrimaryKeyColumn + "=?";
  if(sqlite::Tools::ExecuteUpdate(m_ml->GetConnection(), req, value, m_id) == false)
    return false;
  m_isHidden = value;
  return true;
}

unsigned int mxp::LibraryFolder::GetPosition() const {
  return m_position;
}

bool mxp::LibraryFolder::SetPosition(unsigned int value) {
  if(m_position == value)
    return true;
  if(value == 0)
    return false;

  static const auto req = "UPDATE " + LibraryFolderTable::Name + " SET position=? WHERE " + LibraryFolderTable::PrimaryKeyColumn + "=?";
  if(sqlite::Tools::ExecuteUpdate(m_ml->GetConnection(), req, value, m_id) == false)
    return false;
  m_position = value;
  return true;
}

bool mxp::LibraryFolder::AddMedia(int64_t mediaId) {
  static const mstring req = "INSERT INTO MediaLibraryFolderRelation(media_id, library_folder_id, position) VALUES(?, ?, ?)";
  try {
    return sqlite::Tools::ExecuteInsert(m_ml->GetConnection(), req, mediaId, m_id, sqlite::ForeignKey{ 0 }) != 0;
  } catch(const sqlite::errors::ConstraintViolation& ex) {
    LOG_WARN("Rejected library folder insertion: ", ex.what());
    return false;
  }
}

bool mxp::LibraryFolder::SetMediaPosition(int64_t mediaId, unsigned int position) {
  if(position == 0)
    return false;

  static const auto req = "UPDATE MediaLibraryFolderRelation SET position=? WHERE library_folder_id=? AND media_id=? AND position!=?";
  if(sqlite::Tools::ExecuteUpdate(m_ml->GetConnection(), req, position, m_id, mediaId, position) == false)
    return false;

  return true;
}

bool mxp::LibraryFolder::RemoveMedia(int64_t mediaId) {
  static const mstring req = "DELETE FROM MediaLibraryFolderRelation WHERE library_folder_id=? AND media_id=?";
  return sqlite::Tools::ExecuteDelete(m_ml->GetConnection(), req, m_id, mediaId);
}

const mxp::LibraryFolderPtr& mxp::LibraryFolder::GetParent() const {
  auto lock = m_parent.Lock();
  if(m_parent.IsCached())
    return m_parent;

  m_parent = Fetch(m_ml, m_parentId);

  return m_parent;
}

int64_t mxp::LibraryFolder::GetParentId() const {
  return m_parentId;
}

bool mxp::LibraryFolder::SetParent(int64_t libraryFolderId) {
  if(m_parentId == libraryFolderId)
    return true;

  auto parent = Fetch(m_ml, libraryFolderId);
  while(parent != nullptr) {
    if (parent->m_id == m_id) {
      return false;
    }
    parent = Fetch(m_ml, parent->m_parentId);
  }

  static const auto req = "UPDATE " + LibraryFolderTable::Name + " SET parent_id=? WHERE " + LibraryFolderTable::PrimaryKeyColumn + "=?";
  if(sqlite::Tools::ExecuteUpdate(m_ml->GetConnection(), req, libraryFolderId, m_id) == false)
    return false;
  
  m_parentId = libraryFolderId;

  return true;
}

std::vector<mxp::LibraryFolderPtr> mxp::LibraryFolder::GetChildren() const {
  static const auto req = "SELECT * FROM " + LibraryFolderTable::Name + " WHERE parent_id=? ORDER BY position";
  return FetchAll<ILibraryFolder>(m_ml, req, m_id);
}

std::vector<mxp::MediaPtr> mxp::LibraryFolder::GetMedia() const {
  // Check for special-case default folders
  if(m_id == AllLibraryFolderID) {
    return Media::ListAll(m_ml, SortingCriteria::Default, false);
  } else if(m_id == VideosLibraryFolderID) {
    return Media::ListAll(m_ml, MediaType::Video, SortingCriteria::Default, false);
  } else if(m_id == AudioLibraryFolderID) {
    return Media::ListAll(m_ml, MediaType::Audio, SortingCriteria::Default, false);
  }

  //auto lock = m_media.Lock();
  //if(m_media.IsCached())
  //  return m_media;

  static const auto req = "SELECT m.* FROM " + policy::MediaTable::Name + " m "
    " INNER JOIN MediaLibraryFolderRelation mlfr ON mlfr.media_id=m." + policy::MediaTable::PrimaryKeyColumn +
    " WHERE mlfr.library_folder_id=?";
  return Media::FetchAll<IMedia>(m_ml, req, m_id);;
  //m_media = Media::FetchAll<IMedia>(m_ml, req, m_id);
  //return m_media;
}

bool mxp::LibraryFolder::CreateTable(DBConnection connection) noexcept {
  static const auto req = "CREATE TABLE IF NOT EXISTS " + LibraryFolderTable::Name + "(" +
    LibraryFolderTable::PrimaryKeyColumn + " INTEGER PRIMARY KEY AUTOINCREMENT,"
    "name TEXT COLLATE NOCASE,"
    "is_virtual BOOLEAN NOT NULL DEFAULT 0,"
    "is_hidden BOOLEAN NOT NULL DEFAULT 0,"
    "position INTEGER,"
    "parent_id INTEGER DEFAULT NULL,"
    "FOREIGN KEY(parent_id) REFERENCES " + LibraryFolderTable::Name + "(" + LibraryFolderTable::PrimaryKeyColumn + ") ON DELETE CASCADE,"
    "CONSTRAINT library_folder_name_unique UNIQUE (name, parent_id)"
    ")";
  static const auto reqRel = "CREATE TABLE IF NOT EXISTS MediaLibraryFolderRelation("
    "media_id INTEGER NOT NULL,"
    "library_folder_id INTEGER,"
    "position INTEGER,"
    "PRIMARY KEY (media_id, library_folder_id),"
    "FOREIGN KEY(media_id) REFERENCES " + policy::MediaTable::Name + "(" + policy::MediaTable::PrimaryKeyColumn + ") ON DELETE CASCADE,"
    "FOREIGN KEY(library_folder_id) REFERENCES " + LibraryFolderTable::Name + "(" + LibraryFolderTable::PrimaryKeyColumn + ") ON DELETE CASCADE"
    ")";

  bool success;
  try {
    success = sqlite::Tools::ExecuteRequest(connection, req) &&
      sqlite::Tools::ExecuteRequest(connection, reqRel);
  } catch(std::exception& ex) {
    LOG_ERROR("Failed to create table for Library Folders: ", ex.what());
    success = false;
  }

  return success;
}

bool mxp::LibraryFolder::CreateTriggers(DBConnection connection) noexcept {
  static const auto req = "CREATE TRIGGER IF NOT EXISTS update_library_folder_order AFTER UPDATE OF position"
    " ON " + LibraryFolderTable::Name + " BEGIN "
    " UPDATE " + LibraryFolderTable::Name + " SET position = position + 1"
    " WHERE ((parent_id IS NULL AND new.parent_id IS NULL) OR (parent_id = new.parent_id))"
    " AND position = new.position"
    // We don't want to trigger a self-update when the insert trigger fires.
    " AND " + LibraryFolderTable::PrimaryKeyColumn + " != new." + LibraryFolderTable::PrimaryKeyColumn + ";"
    " END";
  static const auto autoAppendReq = "CREATE TRIGGER IF NOT EXISTS append_new_library_folder_record AFTER INSERT"
    " ON " + LibraryFolderTable::Name + " WHEN new.position IS NULL BEGIN "
    " UPDATE " + LibraryFolderTable::Name + " SET position = ("
    "   SELECT COUNT(" + LibraryFolderTable::PrimaryKeyColumn + ") FROM " + LibraryFolderTable::Name + 
    "   WHERE ((parent_id IS NULL AND new.parent_id IS NULL) OR (parent_id = new.parent_id))) "
    " WHERE parent_id=new.parent_id AND " + LibraryFolderTable::PrimaryKeyColumn + " = new." + LibraryFolderTable::PrimaryKeyColumn + ";"
    " END";
  static const auto autoShiftPosReq = "CREATE TRIGGER IF NOT EXISTS update_library_folder_order_on_insert AFTER INSERT"
    " ON " + LibraryFolderTable::Name + " WHEN new.position IS NOT NULL BEGIN "
    " UPDATE " + LibraryFolderTable::Name + " SET position = position + 1"
    " WHERE ((parent_id IS NULL AND new.parent_id IS NULL) OR (parent_id = new.parent_id))"
    " AND position = new.position"
    " AND " + LibraryFolderTable::PrimaryKeyColumn + " != new." + LibraryFolderTable::PrimaryKeyColumn + ";"
    " END";
  static const auto req2 = "CREATE TRIGGER IF NOT EXISTS update_media_library_folder_order AFTER UPDATE OF position"
    " ON MediaLibraryFolderRelation BEGIN "
    " UPDATE MediaLibraryFolderRelation SET position = position + 1"
    " WHERE library_folder_id = new.library_folder_id"
    " AND position = new.position"
    // We don't want to trigger a self-update when the insert trigger fires.
    " AND media_id != new.media_id;"
    " END";
  static const auto autoAppendReq2 = "CREATE TRIGGER IF NOT EXISTS append_new_media_library_folder_record AFTER INSERT"
    " ON MediaLibraryFolderRelation WHEN new.position IS NULL BEGIN "
    " UPDATE MediaLibraryFolderRelation SET position = ("
    "   SELECT COUNT(library_folder_id) FROM MediaLibraryFolderRelation "
    "   WHERE library_folder_id = new.library_folder_id) "
    " WHERE library_folder_id=new.library_folder_id AND media_id = new.media_id;"
    " END";
  static const auto autoShiftPosReq2 = "CREATE TRIGGER IF NOT EXISTS update_media_library_folder_order_on_insert AFTER INSERT"
    " ON MediaLibraryFolderRelation WHEN new.position IS NOT NULL BEGIN "
    " UPDATE MediaLibraryFolderRelation SET position = position + 1"
    " WHERE library_folder_id = new.library_folder_id"
    " AND position = new.position"
    " AND media_id != new.media_id;"
    " END";

  bool success; 
  try {
    success = sqlite::Tools::ExecuteRequest(connection, req) &&
      sqlite::Tools::ExecuteRequest(connection, autoAppendReq) &&
      sqlite::Tools::ExecuteRequest(connection, autoShiftPosReq) &&
      sqlite::Tools::ExecuteRequest(connection, req2) &&
      sqlite::Tools::ExecuteRequest(connection, autoAppendReq2) &&
      sqlite::Tools::ExecuteRequest(connection, autoShiftPosReq2);
  } catch(std::exception& ex) {
    LOG_ERROR("Failed to create triggers for Library Folders: ", ex.what());
    success = false;
  }

  return success;
}

mxp::LibraryFolderPtr mxp::LibraryFolder::Create(MediaExplorerPtr ml, const std::string& name, int64_t parentId, unsigned int position, bool isHidden) {
  auto folder = std::make_shared<LibraryFolder>(ml, name, parentId, position, false, isHidden);
  static const auto req = "INSERT INTO " + LibraryFolderTable::Name + "(id_library_folder, name, is_virtual, is_hidden, position, parent_id) VALUES(NULL, ?, ?, ?, ?, ?)";
  if(insert(ml, folder, req, name, false, isHidden, sqlite::ForeignKey{ position }, parentId) == false)
    return nullptr;
  return folder;
  
}

bool mxp::LibraryFolder::CreateDefaultFolders(DBConnection connection) {
  // Don't rely on Artist::create, since we want to insert or do nothing here.
  // This will skip the cache for those new entities, but they will be inserted soon enough anyway.

  static const auto req = "INSERT OR IGNORE INTO " + LibraryFolderTable::Name +
    "(id_library_folder, name, is_virtual, position) VALUES"
    "(?, ?, ?, ?),"
    "(?, ?, ?, ?),"
    "(?, ?, ?, ?)";
  sqlite::Tools::ExecuteInsert(connection, req,
    AllLibraryFolderID,    s_all,   true, 0,
    VideosLibraryFolderID, s_video, true, 1,
    AudioLibraryFolderID,  s_audio, true, 2);

  static const auto req2 = "INSERT OR IGNORE INTO " + LibraryFolderTable::Name +
    "(id_library_folder, name, is_virtual, position, parent_id) VALUES"
    "(?, ?, ?, ?, ?),"
    "(?, ?, ?, ?, ?),"
    "(?, ?, ?, ?, ?)";
  sqlite::Tools::ExecuteInsert(connection, req2,
    MoviesLibraryFolderID, s_movies, true, 0, VideosLibraryFolderID,
    ShowsLibraryFolderID,  s_shows,  true, 1, VideosLibraryFolderID,
    AlbumsLibraryFolderID, s_albums, true, 0, AudioLibraryFolderID);

  return true;
}

std::vector<mxp::LibraryFolderPtr> mxp::LibraryFolder::ListAll(MediaExplorerPtr ml, SortingCriteria sort, bool desc) {
  auto req = "SELECT * FROM " + LibraryFolderTable::Name + " ORDER BY ";
  switch(sort) {
  case SortingCriteria::Alpha:
    req += "name";
    break;
  default:
    req += "parent_id, position";
    break;
  }

  if(desc == true)
    req += " DESC";

  return FetchAll<ILibraryFolder>(ml, req);
}