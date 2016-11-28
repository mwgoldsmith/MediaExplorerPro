/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#include "stdafx.h"
#if HAVE_CONFIG_H
# include "config.h"
#endif

#include "Album.h"
#include "AlbumTrack.h"
#include "Artist.h"
#include "AudioTrack.h"
#include "Container.h"
#include "Label.h"
#include "Media.h"
#include "MediaFile.h"
#include "MediaFolder.h"
#include "Metadata.h"
#include "Movie.h"
#include "ShowEpisode.h"
#include "VideoTrack.h"
#include "database/SqliteTools.h"
#include "logging/Logger.h"
#include "filesystem/IDevice.h"
#include "filesystem/IDirectory.h"
#include "filesystem/IFile.h"

using mxp::policy::MediaTable;
const mstring MediaTable::Name = "Media";
const mstring MediaTable::PrimaryKeyColumn = "id_media";
int64_t mxp::Media::* const MediaTable::PrimaryKey = &mxp::Media::m_id;

mxp::Media::Media(MediaExplorerPtr ml, sqlite::Row& row)
  : m_ml(ml)
  , m_changed(false) {
  row >> m_id
      >> m_type
      >> m_subType
      >> m_containerId
      >> m_duration
      >> m_playCount
      >> m_lastPlayedDate
      >> m_progress
      >> m_rating
      >> m_insertionDate
      >> m_releaseDate
      >> m_thumbnail
      >> m_title
      >> m_filename
      >> m_isFavorite
      >> m_isPresent;
}

mxp::Media::Media(MediaExplorerPtr ml, const mstring& title, MediaType type)
  : m_ml(ml)
  , m_id(0)
  , m_type(type)
  , m_subType(MediaSubType::Unknown)
  , m_containerId(0)
  , m_duration(-1)
  , m_playCount(0)
  , m_lastPlayedDate(0)
  , m_progress(.0f)
  , m_rating(-1)
  , m_insertionDate(time(nullptr))
  , m_releaseDate(0)
  , m_title(title)
  // When creating a Media, meta aren't parsed, and therefor, is the filename
  , m_filename(title)
  , m_isFavorite(false)
  , m_isPresent(true)
  , m_changed(false) {
}

int64_t mxp::Media::GetDuration() const {
  return m_duration;
}

void mxp::Media::SetDuration(int64_t duration) {
  if (m_duration == duration)
    return;
  m_duration = duration;
  m_changed = true;
}

mxp::AlbumTrackPtr mxp::Media::AlbumTrack() const {
  if (m_subType != MediaSubType::AlbumTrack)
    return nullptr;
  auto lock = m_albumTrack.Lock();

  if (m_albumTrack.IsCached() == false)
    m_albumTrack = AlbumTrack::FindByMedia(m_ml, m_id);
  return m_albumTrack.Get();
}

void mxp::Media::SetAlbumTrack(AlbumTrackPtr albumTrack) {
  auto lock = m_albumTrack.Lock();
  m_albumTrack = albumTrack;
  m_subType = MediaSubType::AlbumTrack;
  m_changed = true;
}

mxp::ShowEpisodePtr mxp::Media::ShowEpisode() const {
  if (m_subType != MediaSubType::ShowEpisode)
    return nullptr;

  auto lock = m_showEpisode.Lock();
  if (m_showEpisode.IsCached() == false)
    m_showEpisode = ShowEpisode::FindByMedia(m_ml, m_id);
  return m_showEpisode.Get();
}

void mxp::Media::SetShowEpisode(ShowEpisodePtr episode) {
  auto lock = m_showEpisode.Lock();
  m_showEpisode = episode;
  m_subType = MediaSubType::ShowEpisode;
  m_changed = true;
}

mxp::MoviePtr mxp::Media::Movie() const {
  if (m_subType != MediaSubType::Movie)
    return nullptr;

  auto lock = m_movie.Lock();
  if (m_movie.IsCached() == false)
    m_movie = Movie::FromMedia(m_ml, m_id);

  return m_movie.Get();
}

void mxp::Media::SetMovie(MoviePtr movie) {
  auto lock = m_movie.Lock();
  m_movie = movie;
  m_subType = MediaSubType::Movie;
  m_changed = true;
}

int mxp::Media::GetPlayCount() const {
  return m_playCount;
}

bool mxp::Media::IncreasePlayCount() {
  static const auto req = "UPDATE " + MediaTable::Name + " SET play_count = ?, last_played_date = ? WHERE " + MediaTable::PrimaryKeyColumn + " = ?";
  auto lastPlayedDate = time(nullptr);
  if (sqlite::Tools::ExecuteUpdate(m_ml->GetConnection(), req, m_playCount + 1, lastPlayedDate, m_id) == false)
    return false;
  m_playCount++;
  m_lastPlayedDate = lastPlayedDate;
  return true;
}

float mxp::Media::GetProgress() const {
  return m_progress;
}

bool mxp::Media::SetProgress(float progress) {
  static const auto req = "UPDATE " + MediaTable::Name + " SET progress = ? WHERE " + MediaTable::PrimaryKeyColumn + " = ?";
  if (progress == m_progress || progress < 0 || progress > 1.0)
    return true;
  if (sqlite::Tools::ExecuteUpdate(m_ml->GetConnection(), req, progress, m_id) == false)
    return false;
  m_progress = progress;
  return true;
}

int mxp::Media::Rating() const {
  return m_rating;
}

bool mxp::Media::SetRating(int rating) {
  static const auto req = "UPDATE " + MediaTable::Name + " SET rating = ? WHERE " + MediaTable::PrimaryKeyColumn + " = ?";
  if (m_rating == rating)
    return true;
  if (sqlite::Tools::ExecuteUpdate(m_ml->GetConnection(), req, rating, m_id) == false)
    return false;
  m_rating = rating;
  return true;
}

bool mxp::Media::IsFavorite() const {
  return m_isFavorite;
}

bool mxp::Media::SetFavorite(bool favorite) {
  static const auto req = "UPDATE " + MediaTable::Name + " SET is_favorite = ? WHERE " + MediaTable::PrimaryKeyColumn + " = ?";
  if (m_isFavorite == favorite)
    return true;
  if (sqlite::Tools::ExecuteUpdate(m_ml->GetConnection(), req, favorite, m_id) == false)
    return false;
  m_isFavorite = favorite;
  return true;
}

const std::vector<mxp::MediaFilePtr>& mxp::Media::Files() const {
  auto lock = m_files.Lock();
  if (m_files.IsCached() == false) {
    static const auto req = "SELECT * FROM " + policy::MediaFileTable::Name + " WHERE media_id = ?";
    m_files = MediaFile::FetchAll<IMediaFile>(m_ml, req, m_id);
  }
  return m_files;
}

bool mxp::Media::AddVideoTrack(const mstring& codec, unsigned int width, unsigned int height, float fps, const mstring& language, const mstring& description) {
  return VideoTrack::Create(m_ml, codec, width, height, fps, m_id, language, description) != nullptr;
}

std::vector<mxp::VideoTrackPtr> mxp::Media::VideoTracks() {
  static const auto req = "SELECT * FROM " + policy::VideoTrackTable::Name +
      " WHERE media_id = ?";
  return VideoTrack::FetchAll<IVideoTrack>(m_ml, req, m_id);
}

bool mxp::Media::AddAudioTrack(const mstring& codec, unsigned int bitrate, unsigned int sampleRate, unsigned int numChannels, const mstring& language, const mstring& desc) {
  return AudioTrack::Create(m_ml, codec, bitrate, sampleRate, numChannels, language, desc, m_id) != nullptr;
}

std::vector<mxp::AudioTrackPtr> mxp::Media::AudioTracks() {
  static const auto req = "SELECT * FROM " + policy::AudioTrackTable::Name +
      " WHERE media_id = ?";
  return AudioTrack::FetchAll<IAudioTrack>(m_ml, req, m_id);
}

time_t mxp::Media::InsertionDate() const {
  return m_insertionDate;
}

time_t mxp::Media::ReleaseDate() const {
  return m_releaseDate;
}

void mxp::Media::SetReleaseDate(time_t date) {
  if (m_releaseDate == date)
    return;
  m_releaseDate = date;
  m_changed = true;
}

const mstring& mxp::Media::Thumbnail() {
  return m_thumbnail;
}

void mxp::Media::SetThumbnail(const mstring& thumbnail) {
  if (m_thumbnail == thumbnail)
    return;
  m_thumbnail = thumbnail;
  m_changed = true;
}

bool mxp::Media::Save() {
  static const auto req = "UPDATE " + MediaTable::Name + " SET "
    "type = ?, subtype = ?, duration = ?, progress = ?, release_date = ?, "
    "thumbnail = ?, title = ? WHERE " + MediaTable::PrimaryKeyColumn + " = ?";
  if (m_changed == false)
    return true;
  if (sqlite::Tools::ExecuteUpdate(m_ml->GetConnection(), req, m_type, m_subType, m_duration, m_progress, m_releaseDate, m_thumbnail, m_title, m_id) == false) {
    return false;
  }
  m_changed = false;
  return true;
}

std::shared_ptr<mxp::MediaFile> mxp::Media::AddFile(const fs::IFile& fileFs, MediaFolder& parentFolder, fs::IDirectory& parentFolderFs, mxp::MediaFileType type) {
  auto file = MediaFile::Create(m_ml, m_id, type, fileFs, parentFolder.Id(), parentFolderFs.device()->IsRemovable());
  if (file == nullptr)
    return nullptr;
  auto lock = m_files.Lock();
  if (m_files.IsCached())
    m_files.Get().push_back(file);
  return file;
}

void mxp::Media::RemoveFile(MediaFile& file) {
  file.Destroy();
  auto lock = m_files.Lock();
  if (m_files.IsCached() == false)
    return;
  m_files.Get().erase(std::remove_if(begin(m_files.Get()), end(m_files.Get()), [&file](const MediaFilePtr& f) {
    return f->Id() == file.Id();
  }));
}

int64_t mxp::Media::Id() const {
  return m_id;
}

mxp::MediaType mxp::Media::GetType() {
  return m_type;
}

mxp::MediaSubType mxp::Media::GetSubType() const {
  return m_subType;
}

void mxp::Media::SetContainer(mxp::ContainerPtr container) {
  auto lock = m_container.Lock();
  m_container = container;

  m_changed = true;
}

mxp::ContainerPtr mxp::Media::GetContainer() const {
  auto lock = m_container.Lock();
  if(m_container.IsCached() == false)
    m_container = Container::Fetch(m_ml, m_containerId);

  return m_container.Get();
}

void mxp::Media::SetType(MediaType type) {
  if (m_type != type)
    return;
  m_type = type;
  m_changed = true;
}

const mstring& mxp::Media::GetTitle() const {
  return m_title;
}

void mxp::Media::SetTitle(const mstring &title) {
  if (m_title == title)
    return;
  m_title = title;
  m_changed = true;
}

std::vector<mxp::MetadataPtr> mxp::Media::GetMetadata() const {
  static const auto req = "SELECT m.* FROM " + MediaTable::Name + " c "
    " INNER JOIN ContainerMetadataRelation cmr ON cmr.media_id=c." + MediaTable::PrimaryKeyColumn +
    " INNER JOIN " + policy::MetadataTable::Name + " m ON cmr.metadata_id=m." + policy::MetadataTable::PrimaryKeyColumn +
    " WHERE c." + MediaTable::PrimaryKeyColumn + " = ?";
  return Metadata::FetchAll<IMetadata>(m_ml, req, m_id);
}

bool mxp::Media::AddMetadata(int64_t metadataId) {
  static const mstring req = "INSERT INTO ContainerMetadataRelation(media_id, metadata_id) VALUES(?, ?)";
  return sqlite::Tools::ExecuteInsert(m_ml->GetConnection(), req, m_id, metadataId) != 0;
}

bool mxp::Media::RemoveMetadata(int64_t metadataId) {
  static const mstring req = "DELETE FROM ContainerMetadataRelation WHERE media_id = ? AND metadata_id = ?";
  return sqlite::Tools::ExecuteDelete(m_ml->GetConnection(), req, m_id, metadataId);
}

bool mxp::Media::AddLabel(LabelPtr label) {
  if (m_id == 0 || label->Id() == 0) {
    LOG_ERROR("Both file & label need to be inserted in database before being linked together");
    return false;
  }
  auto req = "INSERT INTO LabelFileRelation VALUES(?, ?)";
  if (sqlite::Tools::ExecuteInsert(m_ml->GetConnection(), req, label->Id(), m_id) == 0)
    return false;
  const auto reqFts = "UPDATE " + MediaTable::Name + "Fts "
    "SET labels = labels || ' ' || ? WHERE rowid = ?";
  return sqlite::Tools::ExecuteUpdate(m_ml->GetConnection(), reqFts, label->Name(), m_id);
}

bool mxp::Media::RemoveLabel(LabelPtr label) {
  if (m_id == 0 || label->Id() == 0) {
    LOG_ERROR("Can't unlink a label/file not inserted in database");
    return false;
  }

  auto req = "DELETE FROM LabelFileRelation WHERE label_id = ? AND media_id = ?";
  if(sqlite::Tools::ExecuteDelete(m_ml->GetConnection(), req, label->Id(), m_id) == false) {
    return false;
  }
  
  const auto reqFts = "UPDATE " + MediaTable::Name + "Fts SET labels = TRIM(REPLACE(labels, ?, '')) WHERE rowid = ?";
  return sqlite::Tools::ExecuteUpdate(m_ml->GetConnection(), reqFts, label->Name(), m_id);
}

std::vector<mxp::LabelPtr> mxp::Media::GetLabels() {
  static const auto req = "SELECT l.* FROM " + policy::LabelTable::Name + " l "
      "INNER JOIN LabelFileRelation lfr ON lfr.label_id = l.id_label "
      "WHERE lfr.media_id = ?";
  return Label::FetchAll<ILabel>(m_ml, req, m_id);
}

//********
// Static methods
std::shared_ptr<mxp::Media> mxp::Media::Create(MediaExplorerPtr ml, MediaType type, const fs::IFile& file) noexcept {
  static const auto req = "INSERT INTO " + MediaTable::Name + "(type, insertion_date, title, filename) VALUES(?, ?, ?, ?)";
  std::shared_ptr<Media> self;

  try {
    self = std::make_shared<Media>(ml, file.GetName(), type);
    if(insert(ml, self, req, type, self->m_insertionDate, self->m_title, self->m_filename) == false) {
      self = nullptr;
    }
  } catch(std::exception& ex) {
    LOG_ERROR(MTEXT("Failed to create Media: "), ex.what());
    self = nullptr;
  }

  return self;
}

bool mxp::Media::CreateTable(DBConnection connection) noexcept {
  static const auto req = "CREATE TABLE IF NOT EXISTS " + MediaTable::Name + "(" +
    MediaTable::PrimaryKeyColumn + " INTEGER PRIMARY KEY AUTOINCREMENT,"
    "type INTEGER,"
    "subtype INTEGER,"
    "container_id UNSIGNED INTEGER,"
    "duration INTEGER DEFAULT -1,"
    "play_count UNSIGNED INTEGER,"
    "last_played_date UNSIGNED INTEGER,"
    "progress REAL,"
    "rating INTEGER DEFAULT -1,"
    "insertion_date UNSIGNED INTEGER,"
    "release_date UNSIGNED INTEGER,"
    "thumbnail TEXT,"
    "title TEXT,"
    "filename TEXT,"
    "is_favorite BOOLEAN NOT NULL DEFAULT 0,"
    "is_present BOOLEAN NOT NULL DEFAULT 1,"
    "FOREIGN KEY (container_id) REFERENCES " + policy::ContainerTable::Name + "(" + policy::ContainerTable::PrimaryKeyColumn + ")"
    ")";
  static const auto indexReq = "CREATE INDEX IF NOT EXISTS index_last_played_date ON " +
    MediaTable::Name + "(last_played_date DESC)";
  static const auto vtableReq = "CREATE VIRTUAL TABLE IF NOT EXISTS " +
    MediaTable::Name + "Fts USING FTS3("
    "title,"
    "labels"
    ")";
  // Even though the metadata is associated with the container, strictly speaking, it seems more appropriate to assign it
  // to the Media object (which would have the Container assigned to it) instead. 
  static const auto relTableReq = "CREATE TABLE IF NOT EXISTS ContainerMetadataRelation("
    "media_id INTEGER,"
    "metadata_id INTEGER,"
    "PRIMARY KEY(media_id, metadata_id),"
    "FOREIGN KEY(media_id) REFERENCES " + MediaTable::Name + "(" + MediaTable::PrimaryKeyColumn + ") ON DELETE CASCADE,"
    "FOREIGN KEY(metadata_id) REFERENCES " + policy::MetadataTable::Name + "(" + policy::MetadataTable::PrimaryKeyColumn + ") ON DELETE CASCADE"
    ")";

  return Create(connection, req, "table", MediaTable::Name) &&
    Create(connection, indexReq, "index", MediaTable::Name) &&
    Create(connection, vtableReq, "virtual table", MediaTable::Name) &&
    Create(connection, relTableReq, "table", "ContainerMetadataRelation");
}

bool mxp::Media::CreateTriggers(DBConnection connection) noexcept {
  static const auto triggerReq = "CREATE TRIGGER IF NOT EXISTS has_files_present AFTER UPDATE OF "
    "is_present ON " + policy::MediaFileTable::Name +
    " BEGIN "
    " UPDATE " + MediaTable::Name + " SET is_present="
    "(SELECT COUNT(id_file) FROM " + policy::MediaFileTable::Name + " WHERE media_id=new.media_id AND is_present=1) "
    "WHERE id_media=new.media_id;"
    " END;";
  static const auto triggerReq2 = "CREATE TRIGGER IF NOT EXISTS cascade_file_deletion AFTER DELETE ON "
    + policy::MediaFileTable::Name +
    " BEGIN "
    " DELETE FROM " + MediaTable::Name + " WHERE "
    "(SELECT COUNT(id_file) FROM " + policy::MediaFileTable::Name + " WHERE media_id=old.media_id) = 0"
    " AND id_media=old.media_id;"
    " END;";
  static const auto vtableInsertTrigger = "CREATE TRIGGER IF NOT EXISTS insert_media_fts"
    " AFTER INSERT ON " + MediaTable::Name +
    " BEGIN"
    " INSERT INTO " + MediaTable::Name + "Fts(rowid,title,labels) VALUES(new.id_media, new.title, '');"
    " END";
  static const auto vtableDeleteTrigger = "CREATE TRIGGER IF NOT EXISTS delete_media_fts"
    " BEFORE DELETE ON " + MediaTable::Name +
    " BEGIN"
    " DELETE FROM " + MediaTable::Name + "Fts WHERE rowid = old.id_media;"
    " END";
  static const auto vtableUpdateTitleTrigger2 = "CREATE TRIGGER IF NOT EXISTS update_media_title_fts"
    " AFTER UPDATE OF title ON " + MediaTable::Name +
    " BEGIN"
    " UPDATE " + MediaTable::Name + "Fts SET title = new.title WHERE rowid = new.id_media;"
    " END";
  return Create(connection, triggerReq, "trigger", policy::MediaFileTable::Name + MTEXT(" AFTER UPDATE")) &&
    Create(connection, triggerReq2, "trigger", policy::MediaFileTable::Name + MTEXT(" AFTER DELETE")) &&
    Create(connection, vtableInsertTrigger, "trigger", MediaTable::Name + MTEXT(" AFTER INSERT")) &&
    Create(connection, vtableDeleteTrigger, "trigger", MediaTable::Name + MTEXT(" BEFORE DELETE")) &&
    Create(connection, vtableUpdateTitleTrigger2, "trigger", MediaTable::Name + MTEXT(" AFTER UPDATE"));
}

std::vector<mxp::MediaPtr> mxp::Media::Search(MediaExplorerPtr ml, const mstring& title) {
  static const auto req = "SELECT * FROM " + MediaTable::Name + " WHERE"
    " " + MediaTable::PrimaryKeyColumn + " IN (SELECT rowid FROM " + MediaTable::Name + "Fts"
    " WHERE " + MediaTable::Name + "Fts MATCH ?)"
    "AND is_present = 1";
  return FetchAll<IMedia>(ml, req, title + "*");
}

std::vector<mxp::MediaPtr> mxp::Media::ListAll(MediaExplorerPtr ml, SortingCriteria sort, bool desc) {
  mstring req;

  if(sort == SortingCriteria::LastModificationDate) {
    req = "SELECT m.* FROM " + MediaTable::Name + " m INNER JOIN "
      + policy::MediaFileTable::Name + " f ON m." + MediaTable::PrimaryKeyColumn + " = f.media_id"
      " WHERE (f.type = ? OR f.type = ?)"
      " ORDER BY f.last_modification_date";
    if(desc == true)
      req += " DESC";

    return FetchAll<IMedia>(ml, req, MediaFileType::Entire, MediaFileType::Main);
  }

  req = "SELECT * FROM " + MediaTable::Name + " WHERE is_present = 1 ORDER BY ";
  switch(sort) {
  case SortingCriteria::Duration:
    req += "duration";
    break;
  case SortingCriteria::InsertionDate:
    req += "insertion_date";
    break;
  case SortingCriteria::ReleaseDate:
    req += "release_date";
    break;
  default:
    req += "title";
    break;
  }
  if(desc == true)
    req += " DESC";

  return FetchAll<IMedia>(ml, req);
}

std::vector<mxp::MediaPtr> mxp::Media::ListAll(mxp::MediaExplorerPtr ml, MediaType type, mxp::SortingCriteria sort, bool desc) {
  mstring req;

  if(sort == SortingCriteria::LastModificationDate) {
    req = "SELECT m.* FROM " + MediaTable::Name + " m INNER JOIN "
      + policy::MediaFileTable::Name + " f ON m." + MediaTable::PrimaryKeyColumn + " = f.media_id"
      " WHERE m.type = ?"
      " AND (f.type = ? OR f.type = ?)"
      " ORDER BY f.last_modification_date";
    if(desc == true)
      req += " DESC";

    return FetchAll<IMedia>(ml, req, type, MediaFileType::Entire, MediaFileType::Main);
  }

  req = "SELECT * FROM " + MediaTable::Name + " WHERE type = ? AND is_present = 1 ORDER BY ";
  switch(sort) {
  case SortingCriteria::Duration:
    req += "duration";
    break;
  case SortingCriteria::InsertionDate:
    req += "insertion_date";
    break;
  case SortingCriteria::ReleaseDate:
    req += "release_date";
    break;
  default:
    req += "title";
    break;
  }
  if(desc == true)
    req += " DESC";

  return FetchAll<IMedia>(ml, req, type);
}

std::vector<mxp::MediaPtr> mxp::Media::FetchHistory(MediaExplorerPtr ml) {
  static const auto req = "SELECT * FROM " + MediaTable::Name +
    " WHERE last_played_date IS NOT NULL"
    " ORDER BY last_played_date DESC LIMIT 100";
  return FetchAll<IMedia>(ml, req);
}

void mxp::Media::ClearHistory(MediaExplorerPtr ml) {
  auto dbConn = ml->GetConnection();
  static const auto req = "UPDATE " + MediaTable::Name + " SET "
    "play_count = 0,"
    "last_played_date = NULL,"
    "progress = 0";
  // Clear the entire cache since quite a few items are now containing invalid info.
  clear();
  sqlite::Tools::ExecuteUpdate(dbConn, req);
}

bool mxp::Media::Create(DBConnection connection, const mstring& sql, const mstring& type, const mstring& name) noexcept {
  bool result;

  try {
    result = sqlite::Tools::ExecuteRequest(connection, sql);
  } catch(std::exception& ex) {
    LOG_ERROR(MTEXT("Failed to create "), type, MTEXT(" for "), name, MTEXT(": "), ex.what());
    result = false;
  }

  return result;
}

const mxp::utils::Guid& mxp::Media::GetGuid() const {
  return m_guid;
}
