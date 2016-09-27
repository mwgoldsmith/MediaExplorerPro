/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#if HAVE_CONFIG_H
# include "config.h"
#endif

#include <algorithm>
#include <ctime>

//#include "Album.h"
//#include "AlbumTrack.h"
//#include "Artist.h"
#include "AudioTrack.h"
#include "Label.h"
#include "Media.h"
#include "MediaFile.h"
#include "MediaFolder.h"
#include "VideoTrack.h"
#include "logging/Logger.h"
//#include "Movie.h"
//#include "ShowEpisode.h"
#include "database/SqliteTools.h"
#include "filesystem/IFile.h"
#include "filesystem/IDirectory.h"
#include "filesystem/IDevice.h"

using mxp::policy::MediaTable;
const std::string MediaTable::Name = "Media";
const std::string MediaTable::PrimaryKeyColumn = "id_media";
int64_t mxp::Media::* const MediaTable::PrimaryKey = &mxp::Media::m_id;

mxp::Media::Media(MediaExplorerPtr ml, sqlite::Row& row)
  : m_ml(ml)
  , m_changed(false) {
  row >> m_id
    >> m_type
    >> m_subType
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

mxp::Media::Media(MediaExplorerPtr ml, const std::string& title, Type type)
  : m_ml(ml)
  , m_id(0)
  , m_type(type)
  , m_subType(SubType::Unknown)
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

std::shared_ptr<mxp::Media> mxp::Media::create(MediaExplorerPtr ml, Type type, const fs::IFile& file) {
  auto self = std::make_shared<Media>(ml, file.name(), type);
  static const std::string req = "INSERT INTO " + MediaTable::Name + "(type, insertion_date, title, filename) VALUES(?, ?, ?, ?)";

  if (insert(ml, self, req, type, self->m_insertionDate, self->m_title, self->m_filename) == false)
    return nullptr;
  return self;
}

int64_t mxp::Media::duration() const {
  return m_duration;
}

void mxp::Media::SetDuration(int64_t duration) {
  if (m_duration == duration)
    return;
  m_duration = duration;
  m_changed = true;
}
/*
AlbumTrackPtr Media::albumTrack() const {
  if (m_subType != SubType::AlbumTrack)
    return nullptr;
  auto lock = m_albumTrack.lock();

  if (m_albumTrack.IsCached() == false)
    m_albumTrack = AlbumTrack::fromMedia(m_ml, m_id);
  return m_albumTrack.get();
}*/
/*
void Media::setAlbumTrack(AlbumTrackPtr albumTrack) {
  auto lock = m_albumTrack.lock();
  m_albumTrack = albumTrack;
  m_subType = SubType::AlbumTrack;
  m_changed = true;
}
*//*
ShowEpisodePtr Media::showEpisode() const {
  if (m_subType != SubType::ShowEpisode)
    return nullptr;

  auto lock = m_showEpisode.lock();
  if (m_showEpisode.IsCached() == false)
    m_showEpisode = ShowEpisode::fromMedia(m_ml, m_id);
  return m_showEpisode.get();
}
*//*
void Media::setShowEpisode(ShowEpisodePtr episode) {
  auto lock = m_showEpisode.lock();
  m_showEpisode = episode;
  m_subType = SubType::ShowEpisode;
  m_changed = true;
}*/
/*
MoviePtr Media::movie() const {
  if (m_subType != SubType::Movie)
    return nullptr;

  auto lock = m_movie.lock();

  if (m_movie.IsCached() == false)
    m_movie = Movie::fromMedia(m_ml, m_id);
  return m_movie.get();
}
*//*
void Media::setMovie(MoviePtr movie) {
  auto lock = m_movie.lock();
  m_movie = movie;
  m_subType = SubType::Movie;
  m_changed = true;
}
*/
int mxp::Media::PlayCount() const {
  return m_playCount;
}

bool mxp::Media::IncreasePlayCount() {
  static const auto req = "UPDATE " + MediaTable::Name + " SET "
      "play_count = ?, last_played_date = ? WHERE id_media = ?";
  auto lastPlayedDate = time(nullptr);
  if (sqlite::Tools::ExecuteUpdate(m_ml->GetConnection(), req, m_playCount + 1, lastPlayedDate, m_id) == false)
    return false;
  m_playCount++;
  m_lastPlayedDate = lastPlayedDate;
  return true;
}

float mxp::Media::progress() const {
  return m_progress;
}

bool mxp::Media::SetProgress(float progress) {
  static const auto req = "UPDATE " + MediaTable::Name + " SET progress = ? WHERE id_media = ?";
  if (progress == m_progress || progress < 0 || progress > 1.0)
    return true;
  if (sqlite::Tools::ExecuteUpdate(m_ml->GetConnection(), req, progress, m_id) == false)
    return false;
  m_progress = progress;
  return true;
}

int mxp::Media::rating() const {
  return m_rating;
}

bool mxp::Media::setRating(int rating) {
  static const auto req = "UPDATE " + MediaTable::Name + " SET rating = ? WHERE id_media = ?";
  if (m_rating == rating)
    return true;
  if (sqlite::Tools::ExecuteUpdate(m_ml->GetConnection(), req, rating, m_id) == false)
    return false;
  m_rating = rating;
  return true;
}

bool mxp::Media::isFavorite() const {
  return m_isFavorite;
}

bool mxp::Media::setFavorite(bool favorite) {
  static const auto req = "UPDATE " + MediaTable::Name + " SET is_favorite = ? WHERE id_media = ?";
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
    static const auto req = "SELECT * FROM " + policy::MediaFileTable::Name
        + " WHERE media_id = ?";
    m_files = MediaFile::FetchAll<IMediaFile>(m_ml, req, m_id);
  }
  return m_files;
}

bool mxp::Media::addVideoTrack(const std::string& codec, unsigned int width, unsigned int height, float fps, const std::string& language, const std::string& description) {
  return VideoTrack::create(m_ml, codec, width, height, fps, m_id, language, description) != nullptr;
}

std::vector<mxp::VideoTrackPtr> mxp::Media::videoTracks() {
  static const auto req = "SELECT * FROM " + policy::VideoTrackTable::Name +
      " WHERE media_id = ?";
  return VideoTrack::FetchAll<IVideoTrack>(m_ml, req, m_id);
}

bool mxp::Media::addAudioTrack(const std::string& codec, unsigned int bitrate, unsigned int sampleRate, unsigned int nbChannels, const std::string& language, const std::string& desc) {
  return AudioTrack::create(m_ml, codec, bitrate, sampleRate, nbChannels, language, desc, m_id) != nullptr;
}

std::vector<mxp::AudioTrackPtr> mxp::Media::audioTracks() {
  static const auto req = "SELECT * FROM " + policy::AudioTrackTable::Name +
      " WHERE media_id = ?";
  return AudioTrack::FetchAll<IAudioTrack>(m_ml, req, m_id);
}

time_t mxp::Media::insertionDate() const {
  return m_insertionDate;
}

time_t mxp::Media::releaseDate() const {
  return m_releaseDate;
}

void mxp::Media::setReleaseDate(unsigned int date) {
  if (m_releaseDate == date)
    return;
  m_releaseDate = date;
  m_changed = true;
}

const std::string& mxp::Media::thumbnail() {
  return m_thumbnail;
}

void mxp::Media::setThumbnail(const std::string& thumbnail) {
  if (m_thumbnail == thumbnail)
    return;
  m_thumbnail = thumbnail;
  m_changed = true;
}

bool mxp::Media::save() {
  static const auto req = "UPDATE " + MediaTable::Name + " SET "
      "type = ?, subtype = ?, duration = ?, progress = ?, release_date = ?, "
      "thumbnail = ?, title = ? WHERE id_media = ?";
  if (m_changed == false)
    return true;
  if (sqlite::Tools::ExecuteUpdate(m_ml->GetConnection(), req, m_type, m_subType, m_duration, m_progress, m_releaseDate, m_thumbnail, m_title, m_id) == false) {
    return false;
  }
  m_changed = false;
  return true;
}

std::shared_ptr<mxp::MediaFile> mxp::Media::addFile(const fs::IFile& fileFs, MediaFolder& parentFolder, fs::IDirectory& parentFolderFs, IMediaFile::Type type) {
  auto file = MediaFile::create(m_ml, m_id, type, fileFs, parentFolder.id(), parentFolderFs.device()->IsRemovable());
  if (file == nullptr)
    return nullptr;
  auto lock = m_files.Lock();
  if (m_files.IsCached())
    m_files.Get().push_back(file);
  return file;
}

void mxp::Media::removeFile(MediaFile& file) {
  file.destroy();
  auto lock = m_files.Lock();
  if (m_files.IsCached() == false)
    return;
  m_files.Get().erase(std::remove_if(begin(m_files.Get()), end(m_files.Get()), [&file](const MediaFilePtr& f) {
    return f->id() == file.id();
  }));
}

std::vector<mxp::MediaPtr> mxp::Media::listAll(mxp::MediaExplorerPtr ml, mxp::IMedia::Type type, mxp::SortingCriteria sort, bool desc) {
  std::string req;

  if (sort == mxp::SortingCriteria::LastModificationDate) {
    req = "SELECT m.* FROM " + MediaTable::Name + " m INNER JOIN "
        + policy::MediaFileTable::Name + " f ON m.id_media = f.media_id"
        " WHERE m.type = ?"
        " AND (f.type = ? OR f.type = ?)"
        " ORDER BY f.last_modification_date";
    if (desc == true)
      req += " DESC";

    return FetchAll<mxp::IMedia>(ml, req, type, mxp::MediaFile::Type::Entire, mxp::MediaFile::Type::Main);
  }

  req = "SELECT * FROM " + MediaTable::Name + " WHERE type = ? AND is_present = 1 ORDER BY ";
  switch (sort) {
  case mxp::SortingCriteria::Duration:
    req += "duration";
    break;
  case mxp::SortingCriteria::InsertionDate:
    req += "insertion_date";
    break;
  case mxp::SortingCriteria::ReleaseDate:
    req += "release_date";
    break;
  default:
    req += "title";
    break;
  }
  if (desc == true)
    req += " DESC";

  return FetchAll<IMedia>(ml, req, type);
}

int64_t mxp::Media::id() const {
  return m_id;
}

mxp::IMedia::Type mxp::Media::type() {
  return m_type;
}

mxp::IMedia::SubType mxp::Media::subType() const {
  return m_subType;
}

void mxp::Media::setType(Type type) {
  if (m_type != type)
    return;
  m_type = type;
  m_changed = true;
}

const std::string& mxp::Media::title() const {
  return m_title;
}

void mxp::Media::setTitle(const std::string &title) {
  if (m_title == title)
    return;
  m_title = title;
  m_changed = true;
}

bool mxp::Media::CreateTable(DBConnection connection) {
  auto req = "CREATE TABLE IF NOT EXISTS " + MediaTable::Name + "("
      "id_media INTEGER PRIMARY KEY AUTOINCREMENT,"
      "type INTEGER,"
      "subtype INTEGER,"
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
      "is_present BOOLEAN NOT NULL DEFAULT 1"
      ")";
  static const auto indexReq = "CREATE INDEX IF NOT EXISTS index_last_played_date ON "
      + MediaTable::Name + "(last_played_date DESC)";
  static const auto vtableReq = "CREATE VIRTUAL TABLE IF NOT EXISTS "
      + MediaTable::Name + "Fts USING FTS3("
      "title,"
      "labels"
      ")";
  return sqlite::Tools::ExecuteRequest(connection, req) &&
      sqlite::Tools::ExecuteRequest(connection, indexReq) &&
      sqlite::Tools::ExecuteRequest(connection, vtableReq);
}

bool mxp::Media::createTriggers(DBConnection connection) {
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
  return sqlite::Tools::ExecuteRequest(connection, triggerReq) &&
      sqlite::Tools::ExecuteRequest(connection, triggerReq2) &&
      sqlite::Tools::ExecuteRequest(connection, vtableInsertTrigger) &&
      sqlite::Tools::ExecuteRequest(connection, vtableDeleteTrigger) &&
      sqlite::Tools::ExecuteRequest(connection, vtableUpdateTitleTrigger2);
}

bool mxp::Media::addLabel(LabelPtr label) {
  if (m_id == 0 || label->id() == 0) {
    LOG_ERROR("Both file & label need to be inserted in database before being linked together");
    return false;
  }
  auto req = "INSERT INTO LabelFileRelation VALUES(?, ?)";
  if (sqlite::Tools::ExecuteInsert(m_ml->GetConnection(), req, label->id(), m_id) == 0)
    return false;
  const auto reqFts = "UPDATE " + MediaTable::Name + "Fts "
    "SET labels = labels || ' ' || ? WHERE rowid = ?";
  return sqlite::Tools::ExecuteUpdate(m_ml->GetConnection(), reqFts, label->name(), m_id);
}

bool mxp::Media::removeLabel(LabelPtr label) {
  if (m_id == 0 || label->id() == 0) {
    LOG_ERROR("Can't unlink a label/file not inserted in database");
    return false;
  }

  auto req = "DELETE FROM LabelFileRelation WHERE label_id = ? AND media_id = ?";
  if(sqlite::Tools::ExecuteDelete(m_ml->GetConnection(), req, label->id(), m_id) == false) {
    return false;
  }

  const auto reqFts = "UPDATE " + MediaTable::Name + "Fts SET labels = TRIM(REPLACE(labels, ?, '')) WHERE rowid = ?";
  return sqlite::Tools::ExecuteUpdate(m_ml->GetConnection(), reqFts, label->name(), m_id);
}

std::vector<mxp::LabelPtr> mxp::Media::labels() {
  static const auto req = "SELECT l.* FROM " + policy::LabelTable::Name + " l "
      "INNER JOIN LabelFileRelation lfr ON lfr.label_id = l.id_label "
      "WHERE lfr.media_id = ?";
  return Label::FetchAll<ILabel>(m_ml, req, m_id);
}

std::vector<mxp::MediaPtr> mxp::Media::search(MediaExplorerPtr ml, const std::string& title) {
  static const auto req = "SELECT * FROM " + MediaTable::Name + " WHERE"
      " id_media IN (SELECT rowid FROM " + MediaTable::Name + "Fts"
      " WHERE " + MediaTable::Name + "Fts MATCH ?)"
      "AND is_present = 1";
  return Media::FetchAll<IMedia>(ml, req, title + "*");
}

std::vector<mxp::MediaPtr> mxp::Media::fetchHistory(MediaExplorerPtr ml) {
  static const auto req = "SELECT * FROM " + MediaTable::Name + 
      " WHERE last_played_date IS NOT NULL"
      " ORDER BY last_played_date DESC LIMIT 100";
  return FetchAll<IMedia>(ml, req);
}

void mxp::Media::clearHistory(MediaExplorerPtr ml) {
  auto dbConn = ml->GetConnection();
  static const auto req = "UPDATE " + MediaTable::Name + " SET "
      "play_count = 0,"
      "last_played_date = NULL,"
      "progress = 0";
  // Clear the entire cache since quite a few items are now containing invalid info.
  clear();
  sqlite::Tools::ExecuteUpdate(dbConn, req);
}

