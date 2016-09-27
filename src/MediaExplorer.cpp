/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#if HAVE_CONFIG_H
# include "config.h"
#endif

#include <algorithm>
#include <functional>
#if defined(WIN32)
#include <direct.h>
#include "Fixup.h"
#else
#include <sys/stat.h>
#endif
#include "AudioTrack.h"
#include "MediaDevice.h"
#include "MediaFile.h"
#include "MediaFolder.h"
#include "Label.h"
#include "Media.h"
#include "MediaExplorer.h"
#include "Playlist.h"
#include "Types.h"
#include "VideoTrack.h"
#include "database/SqliteConnection.h"
#include "discoverer/DiscovererWorker.h"
#include "discoverer/FsDiscoverer.h"
#include "factory/DeviceListerFactory.h"
#include "factory/FileSystemFactory.h"
#include "logging/Logger.h"
#include "mediaexplorer/ILogger.h"
#include "mediaexplorer/ISettings.h"
#include "mediaexplorer/IDeviceLister.h"
#include "metadata_services/MetadataParser.h"
#include "parser/Parser.h"
#include "utils/ModificationsNotifier.h"

std::string mxp::MediaExplorer::s_version = PACKAGE_STRING;

const uint32_t mxp::MediaExplorer::DbModelVersion = 2;

mxp::MediaExplorer::MediaExplorer()
  : m_callbacks{ nullptr } {
  mxp::Log::SetLogLevel(LogLevel::Debug);
}

mxp::MediaExplorer::~MediaExplorer() {
  if (m_discoverer != nullptr)
    m_discoverer->stop();

  if (m_parser != nullptr)
    m_parser->stop();

  mxp::Media::clear();
  mxp::MediaFolder::clear();
  mxp::MediaFile::clear();
  mxp::MediaDevice::clear();
  mxp::Label::clear();
  mxp::Playlist::clear();
  mxp::VideoTrack::clear();
  mxp::AudioTrack::clear();
}

bool mxp::MediaExplorer::Initialize(const std::string& dbPath, IMediaExplorerCb* cb) {
  LOG_DEBUG("Initializing start");

  if (m_deviceLister == nullptr) {
    m_deviceLister = mxp::factory::createDeviceLister();
    if (m_deviceLister == nullptr) {
      LOG_ERROR("No available IDeviceLister was found.");
      return false;
    }
  }

  if (m_fsFactory == nullptr) {
    m_fsFactory.reset(new mxp::factory::FileSystemFactory(m_deviceLister));
    m_fsFactory->Refresh();
  }

  m_callbacks = cb;
  m_db.reset(new SqliteConnection(dbPath));

  StartDeletionNotifier();

  if (CreateAllTables() == false) {
    LOG_ERROR("Failed to create database structure");
    return false;
  }

  if (m_settings.load(m_db.get()) == false) {
    return false;
  } else if (m_settings.dbModelVersion() != DbModelVersion) {
    if (UpdateDatabaseModel(m_settings.dbModelVersion()) == false) {
      return false;
    }
  }
  
  StartDiscoverer();
  StartParser();

  LOG_DEBUG("Initializing end");

  return true;
}

bool mxp::MediaExplorer::Shutdown() {
  LOG_DEBUG("MXP-CS-000 - Shutdown start");
  if (m_discoverer != nullptr) {
    m_discoverer->stop();
  }
  if (m_parser != nullptr) {
    m_parser->stop();
  }
  mxp::Media::clear();
  mxp::MediaFolder::clear();
  mxp::MediaFile::clear();
  mxp::MediaDevice::clear();
  mxp::Label::clear();
  mxp::Playlist::clear();
  
  mxp::VideoTrack::clear();
  mxp::AudioTrack::clear();
  LOG_DEBUG("MXP-CS-900 - Shutdown end");
  return true;
}

std::string& mxp::MediaExplorer::GetVersion() const {
  return s_version;
}

void mxp::MediaExplorer::SetLogger(ILogger* logger) {
  m_logger.reset(std::move(logger));
}

void mxp::MediaExplorer::SetCallbacks(IMediaExplorerCb* cb) {
  m_callbacks = cb;
}

mxp::FileSystemPtr mxp::MediaExplorer::GetFileSystem() const {
  return m_fsFactory;
}

mxp::LabelPtr mxp::MediaExplorer::CreateLabel(const std::string& label) {
  return mxp::Label::create(this, label);
}

bool mxp::MediaExplorer::DeleteLabel(mxp::LabelPtr label) {
  return mxp::Label::destroy(this, label->id());
}

mxp::DBConnection mxp::MediaExplorer::GetConnection() const {
  return m_db.get();
}

mxp::IMediaExplorerCb* mxp::MediaExplorer::GetCallbacks() const {
  return m_callbacks;
}

bool mxp::MediaExplorer::DeleteFolder(const mxp::MediaFolder& folder) {
  return false;
}

std::shared_ptr<mxp::MediaDevice> mxp::MediaExplorer::FindDevice(const std::string& uuid) {
  return mxp::MediaDevice::fromUuid(this, uuid);
}

mxp::PlaylistPtr mxp::MediaExplorer::CreatePlaylist(const std::string& name) { 
  return mxp::Playlist::create(this, name);
}

bool mxp::MediaExplorer::DeletePlaylist(int64_t playlistId) { 
  return mxp::Playlist::destroy(this, playlistId);

}

std::vector<mxp::PlaylistPtr> mxp::MediaExplorer::PlaylistList(mxp::SortingCriteria sort, bool desc) { 
  return mxp::Playlist::listAll(this, sort, desc);
}

mxp::PlaylistPtr mxp::MediaExplorer::Playlist(int64_t id) const { 
  return mxp::Playlist::Fetch(this, id);
}

mxp::MediaSearchAggregate mxp::MediaExplorer::SearchMedia(const std::string& title) const { 
  if (ValidateSearchPattern(title) == false)
    return{};
  auto tmp = mxp::Media::search(this, title);
  mxp::MediaSearchAggregate res;
  for (auto& m : tmp) {
    switch (m->subType()) {
    //case mxp::IMedia::SubType::AlbumTrack:
    //  res.tracks.emplace_back(std::move(m));
    //  break;
    //case mxp::IMedia::SubType::Movie:
    //  res.movies.emplace_back(std::move(m));
    //  break;
    //case mxp::IMedia::SubType::ShowEpisode:
    //  res.episodes.emplace_back(std::move(m));
    //  break;
    default:
      res.others.emplace_back(std::move(m));
      break;
    }
  }
  return res;
}

std::vector<mxp::PlaylistPtr> mxp::MediaExplorer::SearchPlaylists(const std::string& name) const {
  if (ValidateSearchPattern(name) == false)
    return{};
  return mxp::Playlist::search(this, name);
}

//std::vector<mxp::GenrePtr> SearchGenre(const std::string& genre) const { }

mxp::SearchAggregate mxp::MediaExplorer::Search(const std::string& pattern) const { 
  SearchAggregate res;
  //res.albums = searchAlbums(pattern);
  //res.artists = searchArtists(pattern);
  //res.genres = searchGenre(pattern);
  res.media = SearchMedia(pattern);
  res.playlists = SearchPlaylists(pattern);
  return res;
}


std::shared_ptr<mxp::Media> mxp::MediaExplorer::CreateMedia(const mxp::fs::IFile& fileFs, mxp::MediaFolder& parentFolder, mxp::fs::IDirectory& parentFolderFs) {

const std::vector<std::string> supportedVideoExtensions{
  // Videos
  "avi", "3gp", "amv", "asf", "divx", "dv", "flv", "gxf",
  "iso", "m1v", "m2v", "m2t", "m2ts", "m4v", "mkv", "mov",
  "mp2", "mp4", "mpeg", "mpeg1", "mpeg2", "mpeg4", "mpg",
  "mts", "mxf", "nsv", "nuv", "ogm", "ogv", "ogx", "ps",
  "rec", "rm", "rmvb", "tod", "ts", "vob", "vro", "webm", "wmv"
};

  auto type = mxp::IMedia::Type::UnknownType;
  auto ext = fileFs.extension();
  auto predicate = [ext](const std::string& v) {
    return strcasecmp(v.c_str(), ext.c_str()) == 0;
  };

  if (std::find_if(begin(supportedVideoExtensions), end(supportedVideoExtensions), predicate) != end(supportedVideoExtensions)) {
    type = mxp::IMedia::Type::VideoType;
  //} else if (std::find_if(begin(supportedAudioExtensions), end(supportedAudioExtensions), predicate) != end(supportedAudioExtensions)) {
  //  type = mxp::IMedia::Type::AudioType;
  }

  if (type == mxp::IMedia::Type::UnknownType) {
    return nullptr;
  }

  LOG_INFO("Adding ", fileFs.fullPath());
  auto mptr = mxp::Media::create(this, type, fileFs);
  if (mptr == nullptr) {
    LOG_ERROR("Failed to add media ", fileFs.fullPath(), " to the media library");
    return nullptr;
  }

  // For now, assume all media are made of a single file
  auto file = mptr->addFile(fileFs, parentFolder, parentFolderFs, mxp::MediaFile::Type::Entire);
  if (file == nullptr) {
    LOG_ERROR("Failed to add file ", fileFs.fullPath(), " to media #", mptr->id());
    mxp::Media::destroy(this, mptr->id());
    return nullptr;
  }

  m_modificationNotifier->NotifyMediaCreation(mptr);
  if (m_parser != nullptr) {
    m_parser->Parse(mptr, file);
  }

  return mptr;
}

bool mxp::MediaExplorer::DeleteMedia(int64_t mediaId) const {
  return true;
}

std::vector<mxp::MediaPtr> mxp::MediaExplorer::MediaList(mxp::SortingCriteria sort, bool desc) {
  return mxp::Media::listAll(this, mxp::IMedia::Type::VideoType, sort, desc);
}

mxp::MediaPtr mxp::MediaExplorer::Media(int64_t mediaId) const {
  return mxp::Media::Fetch(this, mediaId);
}

void mxp::MediaExplorer::Discover(const std::string& entryPoint) {
  LOG_INFO("Discovering from: ", entryPoint);
  if (m_discoverer != nullptr)
    m_discoverer->Discover(entryPoint);
}

void mxp::MediaExplorer::Reload() {
  if (m_discoverer != nullptr)
    m_discoverer->reload();
}

void mxp::MediaExplorer::Reload(const std::string& entryPoint) {
  if (m_discoverer != nullptr)
    m_discoverer->reload(entryPoint);
}

bool mxp::MediaExplorer::CreateAllTables() {
  auto t = m_db->NewTransaction();
  auto res = MediaDevice::createTable(m_db.get()) &&
    MediaFolder::createTable(m_db.get()) &&
      Media::createTable(m_db.get()) &&
      MediaFile::createTable(m_db.get()) &&
      Label::createTable(m_db.get()) &&
      Playlist::createTable(m_db.get()) &&
      VideoTrack::createTable(m_db.get()) &&
      AudioTrack::createTable(m_db.get()) &&
      Settings::createTable(m_db.get());

  if (res == false) {
    return false;
  }

  t->Commit();

  return true;
}

bool mxp::MediaExplorer::ValidateSearchPattern(const std::string& pattern) const {
  return pattern.size() >= 3;
}

bool mxp::MediaExplorer::UpdateDatabaseModel(unsigned int prevVersion) {
  if (prevVersion == 1) {
    // Way too much differences, introduction of devices, and almost unused in the wild, just drop everything
    std::string req = "PRAGMA writable_schema = 1;"
        "delete from sqlite_master;"
        "PRAGMA writable_schema = 0;";
    if (sqlite::Tools::executeRequest(GetConnection(), req) == false)
      return false;
    if (CreateAllTables() == false)
      return false;
    ++prevVersion;
  }
  // To be continued in the future!

  // Safety check: ensure we didn't forget a migration along the way
  assert(prevVersion == DbModelVersion);
  m_settings.setDbModelVersion(DbModelVersion);
  m_settings.save();

  return true;
}

void mxp::MediaExplorer::StartDiscoverer() {
  m_discoverer.reset(new DiscovererWorker(this));
  m_discoverer->AddDiscoverer(std::unique_ptr<IDiscoverer>(std::make_unique<FsDiscoverer>(m_fsFactory, this, m_callbacks)));
  m_discoverer->reload();
}

void mxp::MediaExplorer::StartParser() {
  m_parser.reset(new Parser(this));

  //auto vlcService = std::unique_ptr<VLCMetadataService>(new VLCMetadataService);
  auto metadataService = std::make_unique<mxp::MetadataParser>();
  //auto thumbnailerService = std::unique_ptr<VLCThumbnailer>(new VLCThumbnailer);
  //m_parser->AddService(std::move(vlcService));
  m_parser->AddService(std::move(metadataService));
  //m_parser->AddService(std::move(thumbnailerService));
  m_parser->Start();
}

void mxp::MediaExplorer::PauseBackgroundOperations() {
  if (m_parser != nullptr)
    m_parser->pause();
}

void mxp::MediaExplorer::ResumeBackgroundOperations() {
  if (m_parser != nullptr)
    m_parser->resume();
}

void mxp::MediaExplorer::StartDeletionNotifier() {
  m_modificationNotifier.reset(new mxp::ModificationNotifier(this));
  m_modificationNotifier->Start();
}

std::shared_ptr<mxp::ModificationNotifier> mxp::MediaExplorer::GetNotifier() const {
  return m_modificationNotifier;
}

extern "C" mxp::IMediaExplorer* mxp_newInstance() {
  return new mxp::MediaExplorer();
}
