/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#ifndef MEDIAEXPLORER_H
#define MEDIAEXPLORER_H

#include <string>
#include <memory>

#include "mediaexplorer/ILogger.h"
#include "mediaexplorer/IMediaExplorer.h"
#include "mediaexplorer/IMediaExplorerCb.h"
#include "Settings.h"
#include "Types.h"

namespace mxp {

class MediaDevice;
class DiscovererWorker;
class MediaFolder;
class Genre;
class Media;
class ModificationNotifier;
class Parser;
class ParserService;
class SqliteConnection;

namespace factory {
class IFileSystem;
}

namespace fs {
class IFile;
class IDirectory;
}

class MediaExplorer : public IMediaExplorer {
public:
  MediaExplorer();
  ~MediaExplorer();

  virtual bool Initialize(const std::string& dbPath, IMediaExplorerCb* cb) override;
  virtual bool Shutdown() override;

  virtual std::string& GetVersion() const override;
  virtual void SetLogger(ILogger* logger) override;
  virtual void SetCallbacks(IMediaExplorerCb* cb) override;
  virtual FileSystemPtr GetFileSystem() const override;

  virtual LabelPtr CreateLabel(const std::string& label) override;
  virtual bool DeleteLabel(LabelPtr label) override;
  
  virtual std::vector<GenrePtr> GenreList(SortingCriteria sort, bool desc) const override;
  virtual GenrePtr Genre(int64_t id) const override;

  bool AddToHistory(const std::string& mrl);

  virtual PlaylistPtr CreatePlaylist(const std::string& name) override;
  virtual bool DeletePlaylist(int64_t playlistId) override;
  virtual std::vector<PlaylistPtr> PlaylistList(SortingCriteria sort, bool desc) override;
  virtual PlaylistPtr Playlist(int64_t id) const override;

  virtual MediaSearchAggregate SearchMedia(const std::string& title) const override;
  virtual std::vector<PlaylistPtr> SearchPlaylists(const std::string& name) const override;
  virtual std::vector<GenrePtr> SearchGenre(const std::string& genre) const override;
  virtual SearchAggregate Search(const std::string& pattern) const override;

  std::shared_ptr<mxp::Media> CreateMedia(const fs::IFile& fileFs, mxp::MediaFolder& parentFolder, fs::IDirectory& parentFolderFs);
  virtual bool DeleteMedia(int64_t mediaId) const override;
  virtual std::vector<MediaPtr> MediaList(SortingCriteria sort, bool desc) override;
  virtual MediaPtr Media(int64_t mediaId) const override;

  DBConnection GetConnection() const;
  IMediaExplorerCb* GetCallbacks() const;
  std::shared_ptr<ModificationNotifier> GetNotifier() const;

  std::shared_ptr<MediaDevice> FindDevice(const std::string& uuid);

  bool DeleteFolder(const MediaFolder& folder);

  virtual void Reload() override;
  virtual void Reload(const std::string& entryPoint) override;
  virtual void Discover(const std::string& entryPoint) override;

  virtual void PauseBackgroundOperations() override;
  virtual void ResumeBackgroundOperations() override;

public:
  static const uint32_t DbModelVersion;

private:
  bool CreateAllTables();
  bool UpdateDatabaseModel(unsigned int prevVersion);

  virtual void StartDiscoverer();
  virtual void StartParser();
  virtual void StartDeletionNotifier();

  bool ValidateSearchPattern(const std::string& pattern) const;

protected:
  Settings                              m_settings;
  std::unique_ptr<SqliteConnection>     m_db;
  std::unique_ptr<ILogger>              m_logger;
  std::unique_ptr<Parser>               m_parser;
  std::unique_ptr<DiscovererWorker>     m_discoverer;
  std::shared_ptr<ModificationNotifier> m_modificationNotifier;
  DeviceListerPtr                       m_deviceLister;
  FileSystemPtr                         m_fsFactory;
  IMediaExplorerCb*                     m_callbacks;

private:
  static std::string                 s_version;
};

}

#endif /* MEDIAEXPLORER_H */