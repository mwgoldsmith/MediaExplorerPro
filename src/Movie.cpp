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

const mstring mxp::policy::MovieTable::Name = "Movie";
const mstring mxp::policy::MovieTable::PrimaryKeyColumn = "id_movie";
int64_t mxp::Movie::* const mxp::policy::MovieTable::PrimaryKey = &mxp::Movie::m_id;

mxp::Movie::Movie(MediaExplorerPtr ml, sqlite::Row& row)
  : m_ml(ml) {
    row >> m_id
        >> m_mediaId
        >> m_title
        >> m_summary
        >> m_artworkMrl
        >> m_imdbId;
}

mxp::Movie::Movie(MediaExplorerPtr ml, int64_t mediaId, const mstring& title)
  : m_ml(ml)
  , m_id(0)
  , m_mediaId(mediaId)
  , m_title(title) {
}

int64_t mxp::Movie::Id() const {
  return m_id;
}

const mstring& mxp::Movie::GetTitle() const {
  return m_title;
}

const mstring& mxp::Movie::GetShortSummary() const {
  return m_summary;
}

bool mxp::Movie::SetShortSummary(const mstring& summary) {
  static const auto req = "UPDATE " + policy::MovieTable::Name + " SET summary=? WHERE " + policy::MovieTable::PrimaryKeyColumn + "=?";
  if(sqlite::Tools::ExecuteUpdate(m_ml->GetConnection(), req, summary, m_id) == false)
    return false;
  m_summary = summary;
  return true;
}

const mstring& mxp::Movie::GetArtworkMrl() const {
  return m_artworkMrl;
}

bool mxp::Movie::SetArtworkMrl(const mstring& artworkMrl) {
  static const auto req = "UPDATE " + policy::MovieTable::Name + " SET artwork_mrl=? WHERE " + policy::MovieTable::PrimaryKeyColumn + "=?";
  if(sqlite::Tools::ExecuteUpdate(m_ml->GetConnection(), req, artworkMrl, m_id) == false)
    return false;
  m_artworkMrl = artworkMrl;
  return true;
}

const mstring& mxp::Movie::GetImdbId() const {
  return m_imdbId;
}

bool mxp::Movie::SetImdbId(const mstring& imdbId) {
  static const auto req = "UPDATE " + policy::MovieTable::Name + " SET imdb_id=? WHERE " + policy::MovieTable::PrimaryKeyColumn + "=?";
  if(sqlite::Tools::ExecuteUpdate(m_ml->GetConnection(), req, imdbId, m_id) == false)
    return false;
  m_imdbId = imdbId;
  return true;
}

std::vector<mxp::MediaPtr> mxp::Movie::GetMedia() {
  static const auto req = "SELECT * FROM " + policy::MediaTable::Name + " WHERE movie_id = ?";
  return Media::FetchAll<IMedia>(m_ml, req, m_id);
}

bool mxp::Movie::CreateTable(DBConnection connection) {
  static const auto req = "CREATE TABLE IF NOT EXISTS " + policy::MovieTable::Name + "(" +
    policy::MovieTable::PrimaryKeyColumn + " INTEGER PRIMARY KEY AUTOINCREMENT,"
    "media_id UNSIGNED INTEGER NOT NULL,"
    "title TEXT UNIQUE ON CONFLICT FAIL,"
    "summary TEXT,"
    "artwork_mrl TEXT,"
    "imdb_id TEXT,"
    "FOREIGN KEY(media_id) REFERENCES " + policy::MediaTable::Name + "(" + policy::MediaTable::PrimaryKeyColumn + ") ON DELETE CASCADE"
    ")";
  return sqlite::Tools::ExecuteRequest(connection, req);
}

std::shared_ptr<mxp::Movie> mxp::Movie::Create(MediaExplorerPtr ml, int64_t mediaId, const mstring& title) {
  auto movie = std::make_shared<Movie>(ml, mediaId, title);
  static const auto req = "INSERT INTO " + policy::MovieTable::Name + "(media_id, title) VALUES(?, ?)";
  if(insert(ml, movie, req, mediaId, title) == false)
    return nullptr;
  return movie;
}

mxp::MoviePtr mxp::Movie::FromMedia(MediaExplorerPtr ml, int64_t mediaId) {
  static const auto req = "SELECT * FROM " + policy::MovieTable::Name + " WHERE media_id = ?";
  return Fetch(ml, req, mediaId);
}

