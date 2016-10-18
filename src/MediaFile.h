/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#ifndef MEDIAFILE_H
#define MEDIAFILE_H

#include "mediaexplorer/IMediaFile.h"
#include "database/DatabaseHelpers.h"
#include "filesystem/IFile.h"
#include "utils/Cache.h"

namespace mxp {

class MediaFile;
class Media;

namespace policy {
struct MediaFileTable {
  static const std::string Name;
  static const std::string PrimaryKeyColumn;
  static int64_t MediaFile::*const PrimaryKey;
};
}

class MediaFile : public IMediaFile, public DatabaseHelpers<MediaFile, policy::MediaFileTable> {
public:
  MediaFile(MediaExplorerPtr ml, sqlite::Row& row);

  MediaFile(MediaExplorerPtr ml, int64_t mediaId, Type type, const fs::IFile& file, int64_t folderId, bool isRemovable);

  virtual int64_t Id() const override;

  virtual const std::string& mrl() const override;

  virtual Type GetType() const override;

  virtual time_t LastModificationDate() const override;

  // Explicitely mark a media as fully parsed, meaning no metadata service needs to run anymore.
  //FIXME: This lacks granularity as we don't have a straight forward way to know which service
  //needs to run or not.
  void markParsed();

  bool isParsed() const;

  std::shared_ptr<Media> media() const;

  bool destroy();

  static bool CreateTable(DBConnection dbConnection) noexcept;

  static std::shared_ptr<MediaFile> Create(MediaExplorerPtr ml, int64_t mediaId, Type type, const fs::IFile& file, int64_t folderId, bool isRemovable) noexcept;

  /**
  * @brief fromPath  Attempts to fetch a file using its full path
  * This will only work if the file was stored on a non removable device
  * @param path      The full path to the wanted file
  * @return          A pointer to the wanted file, or nullptr if it wasn't found
  */
  static std::shared_ptr<MediaFile> FindByPath(MediaExplorerPtr ml, const std::string& path);

  /**
  * @brief fromFileName  Attemps to fetch a file based on its filename and folder id
  * @param ml
  * @param fileName
  * @param folderId
  * @return
  */
  static std::shared_ptr<MediaFile> FindByFileName(MediaExplorerPtr ml, const std::string& fileName, int64_t folderId);

private:
  MediaExplorerPtr m_ml;
  int64_t          m_id;
  int64_t          m_mediaId;
  std::string      m_mrl;
  Type             m_type;
  time_t           m_lastModificationDate;
  bool             m_isParsed;
  int64_t          m_folderId;
  bool             m_isPresent;
  bool             m_isRemovable;

  mutable Cache<std::string>          m_fullPath;
  mutable Cache<std::weak_ptr<Media>> m_media;

  friend policy::MediaFileTable;
};

} /* namespace mxp */

#endif /* MEDIAFILE_H */
