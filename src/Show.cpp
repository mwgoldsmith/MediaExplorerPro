/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#include "stdafx.h"
#if HAVE_CONFIG_H
# include "config.h"
#endif

#include "Media.h"
#include "MediaExplorer.h"
#include "Show.h"
#include "ShowEpisode.h"
#include "database/SqliteTools.h"

const std::string mxp::policy::ShowTable::Name = "Show";
const std::string mxp::policy::ShowTable::PrimaryKeyColumn = "id_show";
int64_t mxp::Show::* const mxp::policy::ShowTable::PrimaryKey = &mxp::Show::m_id;

mxp::Show::Show(MediaExplorerPtr ml, sqlite::Row& row)
  : m_ml(ml) {
  row >> m_id
      >> m_name
      >> m_releaseDate
      >> m_shortSummary
      >> m_artworkMrl
      >> m_tvdbId;
}

mxp::Show::Show(MediaExplorerPtr ml, const std::string& name)
  : m_ml(ml)
  , m_id(0)
  , m_name(name)
  , m_releaseDate(0) {
}

int64_t mxp::Show::Id() const {
  return m_id;
}

const std::string& mxp::Show::GetName() const {
  return m_name;
}

time_t mxp::Show::GetReleaseDate() const {
  return m_releaseDate;
}

bool mxp::Show::SetReleaseDate(time_t date) {
  static const auto req = "UPDATE " + policy::ShowTable::Name + " SET release_date = ? WHERE " + policy::ShowTable::PrimaryKeyColumn + " = ?";
  if(sqlite::Tools::ExecuteUpdate(m_ml->GetConnection(), req, date, m_id) == false)
    return false;
  m_releaseDate = date;
  return true;
}

const std::string& mxp::Show::GetShortSummary() const {
  return m_shortSummary;
}

bool mxp::Show::SetShortSummary(const std::string& summary) {
  static const auto req = "UPDATE " + policy::ShowTable::Name + " SET short_summary = ? WHERE " + policy::ShowTable::PrimaryKeyColumn + " = ?";
  if(sqlite::Tools::ExecuteUpdate(m_ml->GetConnection(), req, summary, m_id) == false)
    return false;
  m_shortSummary = summary;
  return true;
}

const std::string& mxp::Show::GetArtworkMrl() const {
  return m_artworkMrl;
}

bool mxp::Show::SetArtworkMrl(const std::string& artworkMrl) {
  static const auto req = "UPDATE " + policy::ShowTable::Name + " SET artwork_mrl = ? WHERE " + policy::ShowTable::PrimaryKeyColumn + " = ?";
  if(sqlite::Tools::ExecuteUpdate(m_ml->GetConnection(), req, artworkMrl, m_id) == false)
    return false;
  m_artworkMrl = artworkMrl;
  return true;
}

const std::string& mxp::Show::GetTvdbId() {
  return m_tvdbId;
}

bool mxp::Show::SetTvdbId(const std::string& tvdbId) {
  static const auto req = "UPDATE " + policy::ShowTable::Name + " SET tvdb_id = ? WHERE " + policy::ShowTable::PrimaryKeyColumn + " = ?";
  if(sqlite::Tools::ExecuteUpdate(m_ml->GetConnection(), req, tvdbId, m_id) == false)
    return false;
  m_tvdbId = tvdbId;
  return true;
}

std::shared_ptr<mxp::ShowEpisode> mxp::Show::AddEpisode(Media& media, const std::string& title, unsigned int episodeNumber) {
  auto episode = ShowEpisode::Create(m_ml, media.Id(), title, episodeNumber, m_id);
  media.SetShowEpisode(episode);
  media.Save();
  return episode;
}

std::vector<mxp::ShowEpisodePtr> mxp::Show::GetEpisodes() {
  static const auto req = "SELECT * FROM " + policy::ShowEpisodeTable::Name + " WHERE show_id = ?";
  return ShowEpisode::FetchAll<IShowEpisode>(m_ml, req, m_id);
}

bool mxp::Show::CreateTable(DBConnection connection) {
  const auto req = "CREATE TABLE IF NOT EXISTS " + policy::ShowTable::Name + "(" +
    policy::ShowTable::PrimaryKeyColumn + " INTEGER PRIMARY KEY AUTOINCREMENT,"
    "name TEXT, "
    "release_date UNSIGNED INTEGER,"
    "short_summary TEXT,"
    "artwork_mrl TEXT,"
    "tvdb_id TEXT"
    ")";
  return sqlite::Tools::ExecuteRequest(connection, req);
}

std::shared_ptr<mxp::Show> mxp::Show::Create(MediaExplorerPtr ml, const std::string& name) {
  auto show = std::make_shared<Show>(ml, name);
  static const auto req = "INSERT INTO " + policy::ShowTable::Name + "(name) VALUES(?)";
  if(insert(ml, show, req, name) == false)
    return nullptr;
  return show;
}
