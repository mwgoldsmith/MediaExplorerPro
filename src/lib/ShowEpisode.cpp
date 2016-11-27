/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#include "stdafx.h"
#if HAVE_CONFIG_H
# include "config.h"
#endif

#include "Media.h"
#include "Show.h"
#include "ShowEpisode.h"
#include "database/SqliteTools.h"

namespace mxp {

const std::string policy::ShowEpisodeTable::Name = "ShowEpisode";
const std::string policy::ShowEpisodeTable::PrimaryKeyColumn = "show_id";
int64_t ShowEpisode::* const policy::ShowEpisodeTable::PrimaryKey = &ShowEpisode::m_id;

ShowEpisode::ShowEpisode(MediaExplorerPtr ml, sqlite::Row& row)
  : m_ml(ml) {
  row >> m_id
    >> m_mediaId
    >> m_artworkMrl
    >> m_episodeNumber
    >> m_name
    >> m_seasonNumber
    >> m_shortSummary
    >> m_tvdbId
    >> m_showId;
}

ShowEpisode::ShowEpisode(MediaExplorerPtr ml, int64_t mediaId, const std::string& name, unsigned int episodeNumber, int64_t showId)
  : m_ml(ml)
  , m_id(0)
  , m_mediaId(mediaId)
  , m_episodeNumber(episodeNumber)
  , m_name(name)
  , m_seasonNumber(0)
  , m_showId(showId) {
}

int64_t ShowEpisode::Id() const {
  return m_id;
}

const std::string& ShowEpisode::GetArtworkMrl() const {
  return m_artworkMrl;
}

bool ShowEpisode::SetArtworkMrl(const std::string& artworkMrl) {
  static const std::string req = "UPDATE " + policy::ShowEpisodeTable::Name
    + " SET artwork_mrl = ? WHERE id_episode = ?";
  if(sqlite::Tools::ExecuteUpdate(m_ml->GetConnection(), req, artworkMrl, m_id) == false)
    return false;
  m_artworkMrl = artworkMrl;
  return true;
}

unsigned int ShowEpisode::episodeNumber() const {
  return m_episodeNumber;
}

const std::string& ShowEpisode::GetName() const {
  return m_name;
}

unsigned int ShowEpisode::seasonNumber() const {
  return m_seasonNumber;
}

bool ShowEpisode::setSeasonNumber(unsigned int seasonNumber) {
  static const std::string req = "UPDATE " + policy::ShowEpisodeTable::Name
    + " SET season_number = ? WHERE id_episode = ?";
  if(sqlite::Tools::ExecuteUpdate(m_ml->GetConnection(), req, seasonNumber, m_id) == false)
    return false;
  m_seasonNumber = seasonNumber;
  return true;
}

const std::string& ShowEpisode::GetShortSummary() const {
  return m_shortSummary;
}

bool ShowEpisode::SetShortSummary(const std::string& summary) {
  static const std::string req = "UPDATE " + policy::ShowEpisodeTable::Name
    + " SET episode_summary = ? WHERE id_episode = ?";
  if(sqlite::Tools::ExecuteUpdate(m_ml->GetConnection(), req, summary, m_id) == false)
    return false;
  m_shortSummary = summary;
  return true;
}

const std::string& ShowEpisode::GetTvdbId() const {
  return m_tvdbId;
}

bool ShowEpisode::SetTvdbId(const std::string& tvdbId) {
  static const std::string req = "UPDATE " + policy::ShowEpisodeTable::Name
    + " SET tvdb_id = ? WHERE id_episode = ?";
  if(sqlite::Tools::ExecuteUpdate(m_ml->GetConnection(), req, tvdbId, m_id) == false)
    return false;
  m_tvdbId = tvdbId;
  return true;
}

std::shared_ptr<IShow> ShowEpisode::show() {
  if(m_show == nullptr && m_showId != 0) {
    m_show = Show::Fetch(m_ml, m_showId);
  }
  return m_show;
}

std::vector<MediaPtr> ShowEpisode::files() {
  static const std::string req = "SELECT * FROM " + policy::MediaTable::Name
    + " WHERE show_episode_id = ?";
  return Media::FetchAll<IMedia>(m_ml, req, m_id);
}

bool ShowEpisode::CreateTable(DBConnection dbConnection) {
  const std::string req = "CREATE TABLE IF NOT EXISTS " + policy::ShowEpisodeTable::Name
    + "("
    "id_episode INTEGER PRIMARY KEY AUTOINCREMENT,"
    "media_id UNSIGNED INTEGER NOT NULL,"
    "artwork_mrl TEXT,"
    "episode_number UNSIGNED INT,"
    "title TEXT,"
    "season_number UNSIGNED INT,"
    "episode_summary TEXT,"
    "tvdb_id TEXT,"
    "show_id UNSIGNED INT,"
    "FOREIGN KEY(media_id) REFERENCES " + policy::MediaTable::Name
    + "(id_media) ON DELETE CASCADE,"
    "FOREIGN KEY(show_id) REFERENCES " + policy::ShowTable::Name + "(id_show)"
    ")";
  return sqlite::Tools::ExecuteRequest(dbConnection, req);
}

std::shared_ptr<ShowEpisode> ShowEpisode::Create(MediaExplorerPtr ml, int64_t mediaId, const std::string& title, unsigned int episodeNumber, int64_t showId) {
  auto episode = std::make_shared<ShowEpisode>(ml, mediaId, title, episodeNumber, showId);
  static const std::string req = "INSERT INTO " + policy::ShowEpisodeTable::Name
    + "(media_id, episode_number, title, show_id) VALUES(?, ? , ?, ?)";
  if(insert(ml, episode, req, mediaId, episodeNumber, title, showId) == false)
    return nullptr;
  return episode;
}

ShowEpisodePtr ShowEpisode::FindByMedia(MediaExplorerPtr ml, int64_t mediaId) {
  static const std::string req = "SELECT * FROM " + policy::ShowEpisodeTable::Name + " WHERE media_id = ?";
  return Fetch(ml, req, mediaId);
}

}
