/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#ifndef FILE_H
#define FILE_H

#include "mediaexplorer/IFile.h"
#include "database/DatabaseHelpers.h"
#include "database/SqliteConnection.h"
#include "filesystem/IFile.h"
#include "utils/Cache.h"

namespace mxp {

class File;
class Media;

namespace policy {
struct FileTable {
  static const std::string Name;
  static const std::string PrimaryKeyColumn;
  static int64_t File::*const PrimaryKey;
};
}

class File : public IFile, public DatabaseHelpers<File, policy::FileTable> {
public:
  File(MediaExplorerPtr ml, sqlite::Row& row);
  File(MediaExplorerPtr ml, int64_t mediaId, Type type, const fs::IFile& file, int64_t folderId, bool isRemovable);
  virtual int64_t id() const override;
  virtual const std::string& mrl() const override;
  virtual Type type() const override;
  virtual unsigned int lastModificationDate() const override;
  /// Explicitely mark a media as fully parsed, meaning no metadata service needs to run anymore.
  //FIXME: This lacks granularity as we don't have a straight forward way to know which service
  //needs to run or not.
  void markParsed();
  bool isParsed() const;
  std::shared_ptr<Media> media() const;
  bool destroy();

  static bool createTable(DBConnection dbConnection);
  static std::shared_ptr<File> create(MediaExplorerPtr ml, int64_t mediaId, Type type, const fs::IFile& file, int64_t folderId, bool isRemovable);

private:
  MediaExplorerPtr m_ml;

  int64_t m_id;
  int64_t m_mediaId;
  std::string m_mrl;
  Type m_type;
  unsigned int m_lastModificationDate;
  bool m_isParsed;
  int64_t m_folderId;
  bool m_isPresent;
  bool m_isRemovable;

  mutable Cache<std::string> m_fullPath;
  mutable Cache<std::weak_ptr<Media>> m_media;

  friend policy::FileTable;
};

} /* namespace mxp */

#endif /* FILE_H */