/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#ifndef MEDIAEXPLORER_H
#define MEDIAEXPLORER_H

#include "mediaexplorer/IDeviceListerCb.h"
#include "mediaexplorer/ILogger.h"
#include "mediaexplorer/IMediaExplorer.h"
#include "mediaexplorer/IMediaExplorerCb.h"
#include "Settings.h"
#include "Types.h"

namespace mxp {

class MediaExplorer : public IMediaExplorer, public IDeviceListerCb {
public:
  static const uint32_t DbModelVersion;

public:
  MediaExplorer();
  ~MediaExplorer();

  virtual bool Initialize(const std::string& dbPath, IMediaExplorerCb* cb) override;
  virtual bool Startup() override;
  virtual bool Shutdown() override;

  virtual std::string& GetVersion() const override;
  virtual FileSystemPtr GetFileSystem() const override;
  virtual void SetLogger(ILogger* logger) override;

  // Settings
  virtual bool GetSettingString(SettingsKey key, mstring& value) const override;
  virtual bool GetSettingInt(SettingsKey key, uint32_t& value) const override;
  virtual bool GetSettingBool(SettingsKey key, bool& value) const override;
  virtual bool SetSettingString(SettingsKey key, const mstring& value) override;
  virtual bool SetSettingInt(SettingsKey key, const uint32_t value) override;
  virtual bool SetSettingBool(SettingsKey key, const bool value) override;

  // Artists
  virtual ArtistPtr GetArtist(int64_t id) const override;
  ArtistPtr GetArtist(const std::string& name) const;
  std::shared_ptr<Artist> CreateArtist(const std::string& name);
  virtual std::vector<ArtistPtr> ArtistList(SortingCriteria sort, bool desc) const override;

  // Shows
  virtual ShowPtr GetShow(const std::string& name) const override;
  std::shared_ptr<Show> CreateShow(const std::string& name);

  // Movies
  virtual MoviePtr GetMovie(const std::string& title) const override;
  std::shared_ptr<Movie> CreateMovie(Media& media, const std::string& title);

  // Albums
  virtual AlbumPtr GetAlbum(int64_t id) const override;
  std::shared_ptr<Album> CreateAlbum(const std::string& title);
  virtual std::vector<AlbumPtr> AlbumList(SortingCriteria sort, bool desc) const override;

  // Labels
  virtual LabelPtr CreateLabel(const std::string& label) override;
  virtual bool DeleteLabel(LabelPtr label) override;
  
  // Genres
  virtual std::vector<GenrePtr> GenreList(SortingCriteria sort, bool desc) const override;
  virtual GenrePtr GetGenre(int64_t id) const override;

  // Playlists
  virtual PlaylistPtr CreatePlaylist(const std::string& name) override;
  virtual bool DeletePlaylist(int64_t playlistId) override;
  virtual std::vector<PlaylistPtr> PlaylistList(SortingCriteria sort, bool desc) override;
  virtual PlaylistPtr GetPlaylist(int64_t id) const override;

  // Media
  std::shared_ptr<mxp::Media> CreateMedia(const fs::IFile& fileFs, mxp::MediaFolder& parentFolder, fs::IDirectory& parentFolderFs);
  virtual bool DeleteMedia(int64_t mediaId) const override;
  virtual std::vector<MediaPtr> MediaList(SortingCriteria sort, bool desc) override;
  virtual MediaPtr GetMedia(int64_t mediaId) const override;
  MediaPtr GetMedia(const std::string& mrl) const;
  virtual MediaSearchAggregate SearchMedia(const std::string& title) const override;
  virtual std::vector<PlaylistPtr> SearchPlaylists(const std::string& name) const override;

  // Media Folders
  virtual bool IgnoreFolder(const std::string& path) override;
  virtual bool UnignoreFolder(const std::string& path) override;
  bool DeleteMediaFolder(const MediaFolder& folder);

  // Library Folders
  virtual LibraryFolderPtr CreateLibraryFolder(const std::string& name, int64_t parentId, int16_t position) override;
  virtual bool DeleteLibraryFolder(int64_t libraryFolderId) override;
  virtual std::vector<LibraryFolderPtr> LibraryFolderList(mxp::SortingCriteria sort, bool desc) override;
  virtual LibraryFolderPtr GetLibraryFolder(int64_t libraryFolderId) const override;

  // Searching
  virtual std::vector<AlbumPtr> SearchAlbums(const std::string& pattern) const override;
  virtual std::vector<ArtistPtr> SearchArtists(const std::string& name) const override;
  virtual std::vector<GenrePtr> SearchGenre(const std::string& genre) const override;
  virtual SearchAggregate Search(const std::string& pattern) const override;

  // Codecs
  virtual CodecPtr GetCodec(int64_t codecId) const override;
  virtual std::vector<CodecPtr> CodecList(mxp::SortingCriteria sort, bool desc) const override;

  // Containers
  virtual ContainerPtr GetContainer(int64_t containerId) const override;
  virtual std::vector<ContainerPtr> ContainerList(mxp::SortingCriteria sort, bool desc) const override;

  virtual MetadataPtr GetMetadata(int64_t metadataId) const override;


  virtual void Reload() override;
  virtual void Reload(const std::string& entryPoint) override;
  virtual void Discover(const std::string& entryPoint) override;

  virtual void PauseBackgroundOperations() override;
  virtual void ResumeBackgroundOperations() override;

  bool AddToHistory(const std::string& mrl);

  DBConnection GetConnection() const;
  IMediaExplorerCb* GetCallbacks() const;
  std::shared_ptr<ModificationNotifier> GetNotifier() const;

  std::shared_ptr<MediaDevice> FindMediaDevice(const std::string& uuid) const;


private:
  virtual void OnDevicePlugged(const std::string& uuid, const std::string& mountpoint) override;
  virtual void OnDeviceUnplugged(const std::string& uuid) override;

private:
  void Destruct();
  bool CreateAllTables();
  bool UpdateDatabaseModel(unsigned int prevVersion);
  void RegisterEntityHooks();
  void StartDiscoverer();
  void StartParser();
  void StartDeletionNotifier();

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