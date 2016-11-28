/*****************************************************************************
* Media Explorer
*****************************************************************************/

#ifndef LIBRARYFOLDER_H
#define LIBRARYFOLDER_H

#include "database/DatabaseHelpers.h"
#include "mediaexplorer/ILibraryFolder.h"
#include "mediaexplorer/Common.h"
#include "utils/Cache.h"
#include "Types.h"

namespace mxp {

class Media;
class LibraryFolder;

namespace policy {
struct LibraryFolderTable {
  static const std::string Name;
  static const std::string PrimaryKeyColumn;
  static int64_t LibraryFolder::*const PrimaryKey;
};
}

class LibraryFolder : public ILibraryFolder, public DatabaseHelpers<LibraryFolder, policy::LibraryFolderTable> {
  static const std::string s_all;
  static const std::string s_video;
  static const std::string s_audio;
  static const std::string s_movies;
  static const std::string s_shows;
  static const std::string s_albums;

public:
  LibraryFolder(MediaExplorerPtr ml, sqlite::Row& row);

  LibraryFolder(MediaExplorerPtr ml, const std::string& name, int64_t parentId, unsigned int position, bool isVirtual, bool isHidden);

  virtual int64_t Id() const override;

  virtual const std::string& GetName() const override;

  virtual bool SetName(const std::string& name) override;

  virtual bool IsVirtual() const override;

  virtual bool IsHidden() const override;

  virtual bool SetHidden(bool value) override;

  virtual unsigned int GetPosition() const override;

  virtual bool SetPosition(unsigned int value) override;

  virtual bool AddMedia(int64_t mediaId) override;

  virtual bool SetMediaPosition(int64_t mediaId, unsigned position) override;

  virtual bool RemoveMedia(int64_t mediaId) override;

  virtual const LibraryFolderPtr& GetParent() const override;

  virtual int64_t GetParentId() const override;

  virtual bool SetParent(int64_t libraryFolderId) override;

  virtual std::vector<LibraryFolderPtr> GetChildren() const override;

  virtual std::vector<MediaPtr> GetMedia() const override;

  static bool CreateTable(DBConnection connection) noexcept;

  static bool CreateTriggers(DBConnection connection) noexcept;

  static LibraryFolderPtr Create(MediaExplorerPtr ml, const std::string& name, int64_t parentId, unsigned int position, bool isHidden);

  static bool CreateDefaultFolders(DBConnection connection);

  static std::vector<LibraryFolderPtr> ListAll(MediaExplorerPtr ml, SortingCriteria sort, bool desc);

private:
  MediaExplorerPtr m_ml;
  int64_t          m_id;
  std::string      m_name;
  bool             m_isVirtual;
  bool             m_isHidden;
  unsigned int     m_position;
  int64_t          m_parentId;

  mutable Cache<LibraryFolderPtr> m_parent;
  //mutable Cache<std::vector<MediaPtr>>         m_media;

  friend struct policy::LibraryFolderTable;
};

} /* namespace mxp */

#endif /* LIBRARYFOLDER_H */
