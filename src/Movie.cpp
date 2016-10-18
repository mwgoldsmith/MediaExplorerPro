/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#include "stdafx.h"
#if HAVE_CONFIG_H
# include "config.h"
#endif

#include "Media.h"
#include "Movie.h"
#include "database/SqliteTools.h"

namespace mxp {

const std::string policy::MovieTable::Name = "Movie";
const std::string policy::MovieTable::PrimaryKeyColumn = "id_movie";
int64_t Movie::* const policy::MovieTable::PrimaryKey = &Movie::m_id;

Movie::Movie(MediaExplorerPtr ml, sqlite::Row& row)
  : m_ml(ml) {
    row >> m_id
      >> m_mediaId
      >> m_title
      >> m_summary
      >> m_artworkMrl
      >> m_imdbId;
}

Movie::Movie(MediaExplorerPtr ml, int64_t mediaId, const std::string& title)
  : m_ml(ml)
  , m_id(0)
  , m_mediaId(mediaId)
  , m_title(title) {
}

int64_t Movie::Id() const {
  return m_id;
}

const std::string&Movie::GetTitle() const {
  return m_title;
}

const std::string& Movie::ShortSummary() const {
  return m_summary;
}

bool Movie::SetShortSummary(const std::string& summary) {
  static const std::string req = "UPDATE " + policy::MovieTable::Name
    + " SET summary = ? WHERE id_movie = ?";
  if(sqlite::Tools::ExecuteUpdate(m_ml->GetConnection(), req, summary, m_id) == false)
    return false;
  m_summary = summary;
  return true;
}

const std::string&Movie::GetArtworkMrl() const {
  return m_artworkMrl;
}

bool Movie::SetArtworkMrl(const std::string& artworkMrl) {
  static const std::string req = "UPDATE " + policy::MovieTable::Name
    + " SET artwork_mrl = ? WHERE id_movie = ?";
  if(sqlite::Tools::ExecuteUpdate(m_ml->GetConnection(), req, artworkMrl, m_id) == false)
    return false;
  m_artworkMrl = artworkMrl;
  return true;
}

const std::string& Movie::imdbId() const {
  return m_imdbId;
}

bool Movie::setImdbId(const std::string& imdbId) {
  static const std::string req = "UPDATE " + policy::MovieTable::Name
    + " SET imdb_id = ? WHERE id_movie = ?";
  if(sqlite::Tools::ExecuteUpdate(m_ml->GetConnection(), req, imdbId, m_id) == false)
    return false;
  m_imdbId = imdbId;
  return true;
}

std::vector<MediaPtr> Movie::Files() {
  static const std::string req = "SELECT * FROM " + policy::MediaTable::Name
    + " WHERE movie_id = ?";
  return Media::FetchAll<IMedia>(m_ml, req, m_id);
}

bool Movie::CreateTable(DBConnection dbConnection) {
  static const std::string req = "CREATE TABLE IF NOT EXISTS " + policy::MovieTable::Name
    + "("
    "id_movie INTEGER PRIMARY KEY AUTOINCREMENT,"
    "media_id UNSIGNED INTEGER NOT NULL,"
    "title TEXT UNIQUE ON CONFLICT FAIL,"
    "summary TEXT,"
    "artwork_mrl TEXT,"
    "imdb_id TEXT,"
    "FOREIGN KEY(media_id) REFERENCES " + policy::MediaTable::Name
    + "(id_media) ON DELETE CASCADE"
    ")";
  return sqlite::Tools::ExecuteRequest(dbConnection, req);
}

std::shared_ptr<Movie> Movie::Create(MediaExplorerPtr ml, int64_t mediaId, const std::string& title) {
  auto movie = std::make_shared<Movie>(ml, mediaId, title);
  static const std::string req = "INSERT INTO " + policy::MovieTable::Name
    + "(media_id, title) VALUES(?, ?)";
  if(insert(ml, movie, req, mediaId, title) == false)
    return nullptr;
  return movie;
}

MoviePtr Movie::FromMedia(MediaExplorerPtr ml, int64_t mediaId) {
  static const std::string req = "SELECT * FROM " + policy::MovieTable::Name + " WHERE media_id = ?";
  return Fetch(ml, req, mediaId);
}

}
