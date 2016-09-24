/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#if HAVE_CONFIG_H
# include "config.h"
#endif

#include "File.h"

#include "Media.h"
#include "Folder.h"

namespace mxp {

const std::string policy::FileTable::Name = "File";
const std::string policy::FileTable::PrimaryKeyColumn = "id_file";
int64_t File::* const policy::FileTable::PrimaryKey = &File::m_id;

File::File(MediaExplorerPtr ml, sqlite::Row& row)
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

File::File(MediaExplorerPtr ml, int64_t mediaId, Type type, const fs::IFile& file, int64_t folderId, bool isRemovable)
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

int64_t File::id() const {
  return m_id;
}

const std::string& File::mrl() const {
  if (m_isRemovable == false)
    return m_mrl;

  auto lock = m_fullPath.Lock();
  if (m_fullPath.IsCached())
    return m_fullPath;
  auto folder = Folder::Fetch(m_ml, m_folderId);
  if (folder == nullptr)
    return m_mrl;
  m_fullPath = folder->path() + m_mrl;
  return m_fullPath;
}

IFile::Type File::type() const {
  return m_type;
}

unsigned int File::lastModificationDate() const {
  return m_lastModificationDate;
}

bool File::isParsed() const {
  return m_isParsed;
}

std::shared_ptr<Media> File::media() const {
  auto lock = m_media.Lock();
  if (m_media.IsCached() == false) {
    m_media = Media::Fetch(m_ml, m_mediaId);
  }
  return m_media.Get().lock();
}

bool File::destroy() {
  return DatabaseHelpers::destroy(m_ml, m_id);
}

void File::markParsed() {
  if (m_isParsed == true)
    return;
  static const std::string req = "UPDATE " + policy::FileTable::Name + " SET parsed = 1 WHERE id_file = ?";
  if (sqlite::Tools::executeUpdate(m_ml->GetConnection(), req, m_id) == false)
    return;
  m_isParsed = true;
}

bool File::createTable(DBConnection dbConnection) {
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

std::shared_ptr<File> File::create(MediaExplorerPtr ml, int64_t mediaId, Type type, const fs::IFile& fileFs, int64_t folderId, bool isRemovable) {
  auto self = std::make_shared<File>(ml, mediaId, type, fileFs, folderId, isRemovable);
  static const std::string req = "INSERT INTO " + policy::FileTable::Name +
      "(media_id, mrl, type, folder_id, last_modification_date, is_removable) VALUES(?, ?, ?, ?, ?, ?)";

  if (insert(ml, self, req, mediaId, self->m_mrl, type, sqlite::ForeignKey(folderId),
             self->m_lastModificationDate, isRemovable) == false)
    return nullptr;
  self->m_fullPath = fileFs.fullPath();
  return self;
}

}
