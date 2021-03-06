/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#include "stdafx.h"
#if HAVE_CONFIG_H
# include "config.h"
#endif

#include "Media.h"
#include "MediaFile.h"
#include "MediaFolder.h"
#include "database/SqliteTools.h"

using mxp::policy::MediaFileTable;
const std::string MediaFileTable::Name = "MediaFile";
const std::string MediaFileTable::PrimaryKeyColumn = "id_file";
int64_t mxp::MediaFile::* const MediaFileTable::PrimaryKey = &mxp::MediaFile::m_id;

mxp::MediaFile::MediaFile(MediaExplorerPtr ml, sqlite::Row& row)
  : m_ml(ml)
  , m_id{}
  , m_mediaId{}
  , m_mrl{}
  , m_type{}
  , m_lastModificationDate{}
  , m_numServicesParsed{}
  , m_folderId{}
  , m_isPresent{}
  , m_isRemovable{} {
  row >> m_id
      >> m_mediaId
      >> m_mrl
      >> m_type
      >> m_lastModificationDate
      >> m_numServicesParsed
      >> m_folderId
      >> m_isPresent
      >> m_isRemovable;
}

mxp::MediaFile::MediaFile(MediaExplorerPtr ml, int64_t mediaId, MediaFileType type, const fs::IFile& file, int64_t folderId, bool isRemovable)
  : m_ml(ml)
  , m_id(0)
  , m_mediaId(mediaId)
  , m_mrl(isRemovable == true ? file.GetName() : file.GetFullPath())
  , m_type(type)
  , m_lastModificationDate(file.GetLastModificationDate())
  , m_numServicesParsed(0)
  , m_folderId(folderId)
  , m_isPresent(true)
  , m_isRemovable(isRemovable) {
}

int64_t mxp::MediaFile::Id() const {
  return m_id;
}

const std::string& mxp::MediaFile::GetMrl() const {
  if (m_isRemovable == false)
    return m_mrl;

  auto lock = m_fullPath.Lock();
  if (m_fullPath.IsCached())
    return m_fullPath;

  auto folder = MediaFolder::Fetch(m_ml, m_folderId);
  if (folder == nullptr)
    return m_mrl;

  m_fullPath = folder->path() + m_mrl;

  return m_fullPath;
}

mxp::MediaFileType mxp::MediaFile::GetType() const {
  return m_type;
}

time_t mxp::MediaFile::LastModificationDate() const {
  return m_lastModificationDate;
}

void mxp::MediaFile::SetServicesParsedCount(unsigned int value) {
  if(m_numServicesParsed == value) {
    return;
  }

  static const auto req = "UPDATE " + MediaFileTable::Name + " SET services_parsed_count=? WHERE " + MediaFileTable::PrimaryKeyColumn + "=?";
  if(sqlite::Tools::ExecuteUpdate(m_ml->GetConnection(), req, value, m_id) == false) {
    return;
  }

  m_numServicesParsed = value;
}

unsigned int mxp::MediaFile::GetServicesParsedCount() const {
  return m_numServicesParsed;
}

std::shared_ptr<mxp::Media> mxp::MediaFile::GetMedia() const {
  auto lock = m_media.Lock();
  if (m_media.IsCached() == false) {
    m_media = Media::Fetch(m_ml, m_mediaId);
  }

  return m_media.Get().lock();
}

bool mxp::MediaFile::Destroy() const {
  return DatabaseHelpers::destroy(m_ml, m_id);
}

bool mxp::MediaFile::CreateTable(DBConnection connection) noexcept {
  static const auto req = "CREATE TABLE IF NOT EXISTS " + MediaFileTable::Name + "(" +
    MediaFileTable::PrimaryKeyColumn + " INTEGER PRIMARY KEY AUTOINCREMENT,"
    "media_id INT NOT NULL,"
    "mrl TEXT,"
    "type UNSIGNED INTEGER,"
    "last_modification_date UNSIGNED INT,"
    "services_parsed_count UNSIGNED INT NOT NULL DEFAULT 0,"
    "folder_id UNSIGNED INTEGER,"
    "is_present BOOLEAN NOT NULL DEFAULT 1,"
    "is_removable BOOLEAN NOT NULL,"
    "FOREIGN KEY (media_id) REFERENCES " + policy::MediaTable::Name + "(" + policy::MediaTable::PrimaryKeyColumn + ") ON DELETE CASCADE,"
    "FOREIGN KEY (folder_id) REFERENCES " + policy::MediaFolderTable::Name + "(" + policy::MediaFolderTable::PrimaryKeyColumn +  ") ON DELETE CASCADE,"
    "UNIQUE(mrl, folder_id) ON CONFLICT FAIL"
    ")";
  static const auto triggerReq = "CREATE TRIGGER IF NOT EXISTS is_folder_present AFTER UPDATE OF is_present ON "
    + policy::MediaFolderTable::Name +
    " BEGIN"
    " UPDATE " + MediaFileTable::Name + " SET is_present = new.is_present WHERE folder_id = new.id_folder;"
    " END";
  static const auto indexReq = "CREATE INDEX IF NOT EXISTS file_media_id_index ON " + MediaFileTable::Name + "(media_id)";

  bool success;
  try {
    success = sqlite::Tools::ExecuteRequest(connection, req) &&
      sqlite::Tools::ExecuteRequest(connection, triggerReq) &&
      sqlite::Tools::ExecuteRequest(connection, indexReq);
  } catch (std::exception& ex) {
    LOG_ERROR("Failed to create table for ", policy::MediaTable::Name, ": ", ex.what());
    success = false;
  }

  return success;
}

std::shared_ptr<mxp::MediaFile> mxp::MediaFile::Create(MediaExplorerPtr ml, int64_t mediaId, MediaFileType type, const fs::IFile& fileFs, int64_t folderId, bool isRemovable) noexcept {
  std::shared_ptr<MediaFile> self;
  static const auto req = "INSERT INTO " + MediaFileTable::Name + "(media_id, mrl, type, folder_id, last_modification_date, is_removable) VALUES(?, ?, ?, ?, ?, ?)";

  try {
    self = std::make_shared<MediaFile>(ml, mediaId, type, fileFs, folderId, isRemovable);

#pragma warning( suppress : 4244 )
    if(insert(ml, self, req, mediaId, self->m_mrl, type, sqlite::ForeignKey(folderId), self->m_lastModificationDate, isRemovable) == false) {
      self = nullptr;
    } else {
      self->m_fullPath = fileFs.GetFullPath();
    }
  } catch (std::exception& ex) {
    LOG_ERROR("Failed to create MediaFile: ", ex.what());
    self = nullptr;
  }

  return self;
}

std::shared_ptr<mxp::MediaFile> mxp::MediaFile::FindByPath(MediaExplorerPtr ml, const std::string& path) {
  static const auto req = "SELECT * FROM " + MediaFileTable::Name + " WHERE mrl = ?";
  auto file = Fetch(ml, req, path);
  if(file == nullptr)
    return nullptr;
  // safety checks: since this only works for files on non removable devices, isPresent must be true
  // and isRemovable must be false
  assert(file->m_isPresent == true);
  assert(file->m_isRemovable == false);
  return file;
}

std::shared_ptr<mxp::MediaFile> mxp::MediaFile::FindByFileName(MediaExplorerPtr ml, const std::string& fileName, int64_t folderId) {
  static const auto req = "SELECT * FROM " + MediaFileTable::Name + " WHERE mrl = ? AND folder_id = ?";
  auto file = Fetch(ml, req, fileName, folderId);
  if(file == nullptr)
    return nullptr;

  assert(file->m_isRemovable == true);
  return file;
}
