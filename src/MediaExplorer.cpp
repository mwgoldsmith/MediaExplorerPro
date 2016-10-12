/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#if HAVE_CONFIG_H
# include "config.h"
#endif

#include <algorithm>
#include <functional>
#if defined(_WIN32)
#include "Fixup.h"
#else
#include <sys/stat.h>
#endif
#include "AudioTrack.h"
#include "Codec.h"
#include "Container.h"
#include "Genre.h"
#include "History.h"
#include "Label.h"
#include "Media.h"
#include "MediaDevice.h"
#include "MediaExplorer.h"
#include "MediaFile.h"
#include "MediaFolder.h"
#include "Metadata.h"
#include "Playlist.h"
#include "Stream.h"
#include "Types.h"
#include "VideoTrack.h"
#include "database/SqliteConnection.h"
#include "discoverer/DiscovererWorker.h"
#include "discoverer/FsDiscoverer.h"
#include "factory/DeviceListerFactory.h"
#include "factory/FileSystemFactory.h"
#include "factory/ParserMediaFactory.h"
#include "filesystem/IDevice.h"
#include "logging/Logger.h"
#include "mediaexplorer/ILogger.h"
#include "metadata_services/FormatService.h"
#include "metadata_services/MetadataService.h"
#include "parser/Parser.h"
#include "utils/Filename.h"
#include "utils/ModificationsNotifier.h"
#include "av/AvController.h"

std::string mxp::MediaExplorer::s_version = PACKAGE_STRING;

const uint32_t mxp::MediaExplorer::DbModelVersion = 2;

//********
// Constructors and Destructor
mxp::MediaExplorer::MediaExplorer()
  : m_callbacks{ nullptr } {
  Log::SetLogLevel(LogLevel::Trace);
}

mxp::MediaExplorer::~MediaExplorer() {
  Destruct();
}

//********
// Startup and shutdown
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

  if (m_settings.Load(m_db.get()) == false) {
    return false;
  } else if (m_settings.DbModelVersion() != DbModelVersion) {
    if (UpdateDatabaseModel(m_settings.DbModelVersion()) == false) {
      return false;
    }
  }
  
  StartAvController();
  StartDiscoverer();
  StartParser();

  LOG_DEBUG("Initializing end");

  return true;
}

bool mxp::MediaExplorer::Shutdown() {
  LOG_DEBUG("MXP-CS-000 - Shutdown start");
  Destruct();
  LOG_DEBUG("MXP-CS-900 - Shutdown end");
  return true;
}

void mxp::MediaExplorer::Destruct() {
  if(m_discoverer != nullptr) {
    m_discoverer->stop();
  }

  if(m_parser != nullptr) {
    m_parser->stop();
  }

  Media::clear();
  MediaFolder::clear();
  MediaFile::clear();
  MediaDevice::clear();
  Label::clear();
  Playlist::clear();
  VideoTrack::clear();
  AudioTrack::clear();
  History::clear();
  Genre::clear();
  Codec::clear();
  Container::clear();
  Stream::clear();
  Metadata::clear();
}

//********
// Initialization functions
bool mxp::MediaExplorer::CreateAllTables() {
  try {
    auto t = m_db->NewTransaction();
    auto res = MediaDevice::CreateTable(m_db.get()) &&
      Codec::CreateTable(m_db.get()) &&
      Container::CreateTable(m_db.get()) &&
      Metadata::CreateTable(m_db.get()) &&
      MediaFolder::CreateTable(m_db.get()) &&
      Media::CreateTable(m_db.get()) &&
      MediaFile::CreateTable(m_db.get()) &&
      Stream::CreateTable(m_db.get()) &&
      Label::CreateTable(m_db.get()) &&
      Playlist::CreateTable(m_db.get()) &&
      Genre::CreateTable(m_db.get()) &&
      VideoTrack::CreateTable(m_db.get()) &&
      AudioTrack::CreateTable(m_db.get()) &&
      Media::CreateTriggers(m_db.get()) &&
      Playlist::CreateTriggers(m_db.get()) &&
      History::CreateTable(m_db.get()) &&
      Settings::CreateTable(m_db.get());

    if(res == false) {
      return false;
    }

    t->Commit();
  } catch(std::exception& ex) {
    LOG_ERROR(ex.what());
  }

  return true;
}

bool mxp::MediaExplorer::UpdateDatabaseModel(unsigned int prevVersion) {
  if(prevVersion == 1) {
    // Way too much differences, introduction of devices, and almost unused in the wild, just drop everything
    std::string req = "PRAGMA writable_schema = 1;"
      "delete from sqlite_master;"
      "PRAGMA writable_schema = 0;";
    if(sqlite::Tools::ExecuteRequest(GetConnection(), req) == false)
      return false;
    if(CreateAllTables() == false)
      return false;
    ++prevVersion;
  }
  // To be continued in the future!

  // Safety check: ensure we didn't forget a migration along the way
  assert(prevVersion == DbModelVersion);
  m_settings.SetDbModelVersion(DbModelVersion);
  m_settings.Save();

  return true;
}

void mxp::MediaExplorer::StartAvController() {
  // Database must be initialized already
  av::AvController::Initialize(this);

  if(m_pmFactory == nullptr) {
    m_pmFactory.reset(new factory::ParserMediaFactory(this));
  }
}

void mxp::MediaExplorer::StartDiscoverer() {
  m_discoverer.reset(new DiscovererWorker(this));
  m_discoverer->AddDiscoverer(std::unique_ptr<IDiscoverer>(std::make_unique<FsDiscoverer>(m_fsFactory, this, m_callbacks)));
  m_discoverer->reload();
}

void mxp::MediaExplorer::StartParser() {
  m_parser.reset(new Parser(this));

  auto formatService = std::make_unique<mxp::FormatService>();
  auto metadataService = std::make_unique<mxp::MetadataService>();
  //auto thumbnailerService = std::unique_ptr<VLCThumbnailer>(new VLCThumbnailer);
  m_parser->AddService(std::move(formatService));
  m_parser->AddService(std::move(metadataService));
  //m_parser->AddService(std::move(thumbnailerService));

  m_parser->Start();
}

void mxp::MediaExplorer::StartDeletionNotifier() {
  m_modificationNotifier.reset(new mxp::ModificationNotifier(this));
  m_modificationNotifier->Start();
}

//********
// Labels
mxp::LabelPtr mxp::MediaExplorer::CreateLabel(const std::string& label) {
  return mxp::Label::Create(this, label);
}

bool mxp::MediaExplorer::DeleteLabel(mxp::LabelPtr label) {
  return mxp::Label::destroy(this, label->Id());
}

//********
//Genres
std::vector<mxp::GenrePtr> mxp::MediaExplorer::GenreList(mxp::SortingCriteria sort, bool desc) const {
  return Genre::ListAll(this, sort, desc);
}

mxp::GenrePtr mxp::MediaExplorer::Genre(int64_t id) const {
  return Genre::Fetch(this, id);
}

//********
// Media Folders
bool mxp::MediaExplorer::IgnoreFolder(const std::string& path) {
  return MediaFolder::blacklist(this, path);
}

bool mxp::MediaExplorer::UnignoreFolder(const std::string& path) {
  auto folder = MediaFolder::blacklistedFolder(this, path);
  if(folder == nullptr) {
    return false;
  }

  DeleteMediaFolder(*folder);

  // We are about to refresh the folder we blacklisted earlier, if we don't have a discoverer, stop early
  if(m_discoverer == nullptr) {
    return true;
  }

  auto parentPath = mxp::utils::file::parentDirectory(path);
  // If the parent folder was never added to the media library, the discoverer will reject it.
  // We could check it from here, but that would mean fetching the folder twice, which would be a waste.
  m_discoverer->reload(parentPath);

  return true;
}

bool mxp::MediaExplorer::DeleteMediaFolder(const mxp::MediaFolder& folder) {
  if(MediaFolder::destroy(this, folder.Id()) == false)
    return false;

  Media::clear();

  return true;
}

//********
// Playlists
mxp::PlaylistPtr mxp::MediaExplorer::CreatePlaylist(const std::string& name) { 
  return Playlist::Create(this, name);
}

bool mxp::MediaExplorer::DeletePlaylist(int64_t playlistId) { 
  return Playlist::destroy(this, playlistId);
}

std::vector<mxp::PlaylistPtr> mxp::MediaExplorer::PlaylistList(mxp::SortingCriteria sort, bool desc) { 
  return Playlist::ListAll(this, sort, desc);
}

mxp::PlaylistPtr mxp::MediaExplorer::Playlist(int64_t id) const { 
  return Playlist::Fetch(this, id);
}

//********
// Searching
mxp::MediaSearchAggregate mxp::MediaExplorer::SearchMedia(const std::string& title) const { 
  if (ValidateSearchPattern(title) == false)
    return{};

  auto tmp = Media::Search(this, title);
  MediaSearchAggregate res;
  for (auto& m : tmp) {
    //switch (m->GetSubType()) {
    //case mxp::IMedia::SubType::AlbumTrack:
    //  res.tracks.emplace_back(std::move(m));
    //  break;
    //case mxp::IMedia::SubType::Movie:
    //  res.movies.emplace_back(std::move(m));
    //  break;
    //case mxp::IMedia::SubType::ShowEpisode:
    //  res.episodes.emplace_back(std::move(m));
    //  break;
   // default:
      res.others.emplace_back(std::move(m));
    //  break;
    //}
  }

  return res;
}

std::vector<mxp::PlaylistPtr> mxp::MediaExplorer::SearchPlaylists(const std::string& name) const {
  if (ValidateSearchPattern(name) == false)
    return{};
  return Playlist::Search(this, name);
}

std::vector<mxp::GenrePtr> mxp::MediaExplorer::SearchGenre(const std::string& genre) const {
  if(ValidateSearchPattern(genre) == false)
    return{};
  return Genre::Search(this, genre);
}

mxp::SearchAggregate mxp::MediaExplorer::Search(const std::string& pattern) const { 
  SearchAggregate res;
  //res.albums = searchAlbums(pattern);
  //res.artists = searchArtists(pattern);
  res.genres = SearchGenre(pattern);
  res.media = SearchMedia(pattern);
  res.playlists = SearchPlaylists(pattern);
  return res;
}

//********
// Media
std::shared_ptr<mxp::Media> mxp::MediaExplorer::CreateMedia(const mxp::fs::IFile& fileFs, mxp::MediaFolder& parentFolder, mxp::fs::IDirectory& parentFolderFs) {
  const std::vector<std::string> supportedVideoExtensions{
    // Videos
    "avi", "3gp", "amv", "asf", "divx", "dv", "flv", "gxf",
    "iso", "m1v", "m2v", "m2t", "m2ts", "m4v", "mkv", "mov",
    "mp2", "mp4", "mpeg", "mpeg1", "mpeg2", "mpeg4", "mpg",
    "mts", "mxf", "nsv", "nuv", "ogm", "ogv", "ogx", "ps",
    "rec", "rm", "rmvb", "tod", "ts", "vob", "vro", "webm", "wmv"
  };

  LOG_TRACE("Creating media for ", fileFs.FullPath());
  // TODO: Get container type

  auto type = mxp::IMedia::Type::UnknownType;
  auto ext = fileFs.Extension();
  auto predicate = [ext](const std::string& v) {
    return strcasecmp(v.c_str(), ext.c_str()) == 0;
  };

  if (std::find_if(begin(supportedVideoExtensions), end(supportedVideoExtensions), predicate) != end(supportedVideoExtensions)) {
    type = mxp::IMedia::Type::VideoType;
  //} else if (std::find_if(begin(supportedAudioExtensions), end(supportedAudioExtensions), predicate) != end(supportedAudioExtensions)) {
  //  type = mxp::IMedia::Type::AudioType;
  }

  if (type == mxp::IMedia::Type::UnknownType) {
    LOG_TRACE("Unknown type for file ", fileFs.Name());
    return nullptr;
  }

  LOG_INFO("Adding ", fileFs.FullPath());
  auto mptr = mxp::Media::Create(this, type, fileFs);
  if (mptr == nullptr) {
    LOG_ERROR("Failed to add media ", fileFs.FullPath(), " to the media library");
    return nullptr;
  }

  // For now, assume all media are made of a single file
  auto file = mptr->AddFile(fileFs, parentFolder, parentFolderFs, mxp::MediaFile::Type::Entire);
  if (file == nullptr) {
    LOG_ERROR("Failed to add file ", fileFs.FullPath(), " to media #", mptr->Id());
    mxp::Media::destroy(this, mptr->Id());
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
  return Media::ListAll(this, mxp::IMedia::Type::VideoType, sort, desc);
}

mxp::MediaPtr mxp::MediaExplorer::Media(int64_t mediaId) const {
  return Media::Fetch(this, mediaId);
}

mxp::MediaPtr mxp::MediaExplorer::Media(const std::string& mrl) const {
  auto device = m_fsFactory->CreateDeviceFromPath(mrl);
  if(device == nullptr) {
    LOG_WARN("Failed to create a device associated with mrl ", mrl);
    return nullptr;
  }

  std::shared_ptr<MediaFile> file;
  if(device->IsRemovable() == false) {
    file = MediaFile::FindByPath(this, mrl);
  } else {
    auto folder = MediaFolder::FindByPath(this, utils::file::directory(mrl));
    if(folder == nullptr) {
      LOG_WARN("Failed to find folder containing ", mrl);
      return nullptr;
    }

    if(folder->isPresent() == false) {
      LOG_INFO("Found a folder containing ", mrl, " but it is not present");
      return nullptr;
    }

    file = MediaFile::FindByFileName(this, utils::file::fileName(mrl), folder->Id());
  }

  if(file == nullptr) {
    LOG_WARN("Failed to fetch file for ", mrl, "(device ", device->uuid(), " was ", device->IsRemovable() ? "NOT" : "", "removable)");
    return nullptr;
  }

  return file->media();
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

mxp::DBConnection mxp::MediaExplorer::GetConnection() const {
  return m_db.get();
}

mxp::IMediaExplorerCb* mxp::MediaExplorer::GetCallbacks() const {
  return m_callbacks;
}

bool mxp::MediaExplorer::AddToHistory(const std::string& mrl) {
  return History::insert(GetConnection(), mrl);
}

std::shared_ptr<mxp::MediaDevice> mxp::MediaExplorer::FindMediaDevice(const std::string& uuid) {
  return MediaDevice::FindByUuid(this, uuid);
}

mxp::MetadataPtr mxp::MediaExplorer::Metadata(int64_t metadataId) const {
  return Metadata::Fetch(this, metadataId);
}

mxp::CodecPtr mxp::MediaExplorer::Codec(int64_t codecId) const {
  return Codec::Fetch(this, codecId);
}

std::vector<mxp::CodecPtr> mxp::MediaExplorer::CodecList(mxp::SortingCriteria sort, bool desc) const {
  return Codec::ListAll(this, sort, desc);
}

mxp::ContainerPtr mxp::MediaExplorer::Container(int64_t containerId) const {
  return Container::Fetch(this, containerId);
}

std::vector<mxp::ContainerPtr> mxp::MediaExplorer::ContainerList(mxp::SortingCriteria sort, bool desc) const {
  return Container::ListAll(this, sort, desc);
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

bool mxp::MediaExplorer::ValidateSearchPattern(const std::string& pattern) const {
  return pattern.size() >= 3;
}

void mxp::MediaExplorer::PauseBackgroundOperations() {
  if(m_parser != nullptr) {
    m_parser->pause();
  }
}

void mxp::MediaExplorer::ResumeBackgroundOperations() {
  if(m_parser != nullptr) {
    m_parser->resume();
  }
}

std::shared_ptr<mxp::ModificationNotifier> mxp::MediaExplorer::GetNotifier() const {
  return m_modificationNotifier;
}

std::shared_ptr<mxp::factory::ParserMediaFactory> mxp::MediaExplorer::GetParserMediaFactory() const {
  return m_pmFactory;
}

void mxp::MediaExplorer::OnDevicePlugged(const std::string&, const std::string&) {
  m_fsFactory->Refresh();
}

void mxp::MediaExplorer::OnDeviceUnplugged(const std::string&) {
  m_fsFactory->Refresh();
}

extern "C" {

MXPAPI mxp::IMediaExplorer* mxp_newInstance() {
  return new mxp::MediaExplorer();
}

}