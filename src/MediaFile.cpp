/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#if HAVE_CONFIG_H
# include "config.h"
#endif

#include "MediaFile.h"

#include "Media.h"
#include "MediaFolder.h"

using mxp::policy::FileTable;
const std::string FileTable::Name = "MediaFile";
const std::string FileTable::PrimaryKeyColumn = "id_file";
int64_t mxp::MediaFile::* const FileTable::PrimaryKey = &mxp::MediaFile::m_id;

mxp::MediaFile::MediaFile(MediaExplorerPtr ml, sqlite::Row& row)
  : m_ml(ml) {
  row >> m_id
    >> m_mediaId
    >> m_mrl
    >> m_type
    >> m_lastModificationDate
    >> m_isParsed
    >> m_folderId
    >> m_isPresent
    >> m_isRemovable;
}

mxp::MediaFile::MediaFile(MediaExplorerPtr ml, int64_t mediaId, Type type, const fs::IFile& file, int64_t folderId, bool isRemovable)
  : m_ml(ml)
  , m_id(0)
  , m_mediaId(mediaId)
  , m_mrl(isRemovable == true ? file.name() : file.fullPath())
  , m_type(type)
  , m_lastModificationDate(file.lastModificationDate())
  , m_isParsed(false)
  , m_folderId(folderId)
  , m_isPresent(true)
  , m_isRemovable(isRemovable) {
}

int64_t mxp::MediaFile::id() const {
  return m_id;
}

const std::string& mxp::MediaFile::mrl() const {
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

mxp::IFile::Type mxp::MediaFile::type() const {
  return m_type;
}

time_t mxp::MediaFile::lastModificationDate() const {
  return m_lastModificationDate;
}

bool mxp::MediaFile::isParsed() const {
  return m_isParsed;
}

std::shared_ptr<mxp::Media> mxp::MediaFile::media() const {
  auto lock = m_media.Lock();
  if (m_media.IsCached() == false) {
    m_media = Media::Fetch(m_ml, m_mediaId);
  }
  return m_media.Get().lock();
}

bool mxp::MediaFile::destroy() {
  return DatabaseHelpers::destroy(m_ml, m_id);
}

void mxp::MediaFile::markParsed() {
  if (m_isParsed == true)
    return;
  static const std::string req = "UPDATE " + policy::FileTable::Name + " SET parsed = 1 WHERE id_file = ?";
  if (sqlite::Tools::executeUpdate(m_ml->GetConnection(), req, m_id) == false)
    return;
  m_isParsed = true;
}

bool mxp::MediaFile::createTable(DBConnection dbConnection) {
  std::string req = "CREATE TABLE IF NOT EXISTS " + policy::FileTable::Name + "("
      "id_file INTEGER PRIMARY KEY AUTOINCREMENT,"
      "media_id INT NOT NULL,"
      "mrl TEXT,"
      "type UNSIGNED INTEGER,"
      "last_modification_date UNSIGNED INT,"
      "parsed BOOLEAN NOT NULL DEFAULT 0,"
      "folder_id UNSIGNED INTEGER,"
      "is_present BOOLEAN NOT NULL DEFAULT 1,"
      "is_removable BOOLEAN NOT NULL,"
      "FOREIGN KEY (media_id) REFERENCES " + policy::MediaTable::Name
      + "(id_media) ON DELETE CASCADE,"
      "FOREIGN KEY (folder_id) REFERENCES " + policy::FolderTable::Name
      + "(id_folder) ON DELETE CASCADE,"
      "UNIQUE(mrl, folder_id) ON CONFLICT FAIL"
    ")";
  std::string triggerReq = "CREATE TRIGGER IF NOT EXISTS is_folder_present AFTER UPDATE OF is_present ON "
      + policy::FolderTable::Name +
      " BEGIN"
      " UPDATE " + policy::FileTable::Name + " SET is_present = new.is_present WHERE folder_id = new.id_folder;"
      " END";
  std::string indexReq = "CREATE INDEX IF NOT EXISTS file_media_id_index ON " +
      policy::FileTable::Name + "(media_id)";
  return sqlite::Tools::executeRequest(dbConnection, req) &&
      sqlite::Tools::executeRequest(dbConnection, triggerReq) &&
      sqlite::Tools::executeRequest(dbConnection, indexReq);
}

std::shared_ptr<mxp::MediaFile> mxp::MediaFile::create(MediaExplorerPtr ml, int64_t mediaId, Type type, const fs::IFile& fileFs, int64_t folderId, bool isRemovable) {
  auto self = std::make_shared<MediaFile>(ml, mediaId, type, fileFs, folderId, isRemovable);
  static const std::string req = "INSERT INTO " + policy::FileTable::Name +
      "(media_id, mrl, type, folder_id, last_modification_date, is_removable) VALUES(?, ?, ?, ?, ?, ?)";

  if (insert(ml, self, req, mediaId, self->m_mrl, type, sqlite::ForeignKey(folderId),
             self->m_lastModificationDate, isRemovable) == false)
    return nullptr;
  self->m_fullPath = fileFs.fullPath();
  return self;
}

