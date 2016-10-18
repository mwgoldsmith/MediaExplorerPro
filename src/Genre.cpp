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
#include "Genre.h"
#include "database/SqliteTools.h"

using mxp::policy::GenreTable;
const std::string GenreTable::Name = "Genre";
const std::string GenreTable::PrimaryKeyColumn = "id_genre";
int64_t mxp::Genre::* const GenreTable::PrimaryKey = &mxp::Genre::m_id;

mxp::Genre::Genre(MediaExplorerPtr ml, sqlite::Row& row)
  : m_ml(ml) {
  row >> m_id
      >> m_name;
}

mxp::Genre::Genre(MediaExplorerPtr ml, const std::string& name)
  : m_ml(ml)
  , m_id(0)
  , m_name(name) {}

int64_t mxp::Genre::Id() const {
  return m_id;
}

const std::string& mxp::Genre::Name() const {
  return m_name;
}

std::vector<mxp::ArtistPtr> mxp::Genre::Artists(SortingCriteria, bool desc) const {
  auto req = "SELECT a.* FROM " + policy::ArtistTable::Name + " a "
    "INNER JOIN " + policy::AlbumTrackTable::Name + " att ON att.artist_id = a.id_artist "
    "WHERE att.genre_id = ? GROUP BY att.artist_id"
    " ORDER BY a.name";
  if(desc == true)
    req += " DESC";
  return Artist::FetchAll<IArtist>(m_ml, req, m_id);
}

std::vector<mxp::MediaPtr> mxp::Genre::Tracks(SortingCriteria sort, bool desc) const {
  return AlbumTrack::FindByGenre(m_ml, m_id, sort, desc);
}

std::vector<mxp::AlbumPtr> mxp::Genre::Albums(SortingCriteria sort, bool desc) const {
  return Album::FindByGenre(m_ml, m_id, sort, desc);
}

bool mxp::Genre::CreateTable(DBConnection connection) noexcept {
  static const auto req = "CREATE TABLE IF NOT EXISTS " + GenreTable::Name + "(" +
    GenreTable::PrimaryKeyColumn + " INTEGER PRIMARY KEY AUTOINCREMENT,"
    "name TEXT UNIQUE ON CONFLICT FAIL"
    ")";
#ifdef USE_FTS
  static const auto vtableReq = "CREATE VIRTUAL TABLE IF NOT EXISTS " + 
    GenreTable::Name + "Fts USING FTS3("
    "name"
    ")";
  static const auto vtableInsertTrigger = "CREATE TRIGGER IF NOT EXISTS insert_genre_fts"
    " AFTER INSERT ON " + GenreTable::Name +
    " BEGIN"
    " INSERT INTO " + GenreTable::Name + "Fts(rowid,name) VALUES(new.id_genre, new.name);"
    " END";
  static const auto vtableDeleteTrigger = "CREATE TRIGGER IF NOT EXISTS delete_genre_fts"
    " BEFORE DELETE ON " + GenreTable::Name +
    " BEGIN"
    " DELETE FROM " + GenreTable::Name + "Fts WHERE rowid = old.id_genre;"
    " END";
#endif
  bool success;

  try {
    success = sqlite::Tools::ExecuteRequest(connection, req);
#ifdef USE_FTS
    if(success) {
      success =
        sqlite::Tools::ExecuteRequest(connection, vtableReq) &&
        sqlite::Tools::ExecuteRequest(connection, vtableInsertTrigger) &&
        sqlite::Tools::ExecuteRequest(connection, vtableDeleteTrigger);
    }
#endif
  } catch (std::exception& ex) {
    LOG_ERROR("Failed to create table for Genre: ", ex.what());
    success = false;
  }

  return success;
}

std::shared_ptr<mxp::Genre> mxp::Genre::Create(MediaExplorerPtr ml, const std::string& name) noexcept {
  static const auto req = "INSERT INTO " + GenreTable::Name + "(name) VALUES(?)";
  std::shared_ptr<mxp::Genre> self;

  try {
    self = std::make_shared<Genre>(ml, name);
    if(insert(ml, self, req, name) == false) {
      self = nullptr;
    }
  } catch(std::exception& ex) {
    LOG_ERROR("Failed to create create Genre: ", ex.what());
    self = nullptr;
  }

  return self;
}

std::vector<mxp::GenrePtr> mxp::Genre::Search(MediaExplorerPtr ml, const std::string& name) {
  static const auto req = "SELECT * FROM " + GenreTable::Name + " WHERE "
#ifdef USE_FTS
    "id_genre IN (SELECT rowid FROM " + GenreTable::Name + "Fts WHERE name MATCH ?)";
#else
    "name LIKE '%?%'";
#endif
    auto value = name;
#ifdef USE_FTS
  value += "*";
#endif

  return FetchAll<IGenre>(ml, req,value);
}

std::vector<mxp::GenrePtr> mxp::Genre::ListAll(MediaExplorerPtr ml, SortingCriteria, bool desc) {
  auto req = "SELECT * FROM " + GenreTable::Name + " ORDER BY name";
  if (desc == true)
    req += " DESC";
  return FetchAll<IGenre>(ml, req);
}

std::shared_ptr<mxp::Genre> mxp::Genre::FindByName(MediaExplorerPtr ml, const std::string& name) {
  static const auto req = "SELECT * FROM " + GenreTable::Name + " WHERE name = ?";
  return Fetch(ml, req, name);
}
