/*****************************************************************************
* Media Explorer
*****************************************************************************/

#if HAVE_CONFIG_H
# include "config.h"
#endif

#include "Genre.h"
//#include "Album.h"
//#include "AlbumTrack.h"
//#include "Artist.h"

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
/*
std::vector<ArtistPtr> Genre::artists(SortingCriteria, bool desc) const {
  std::string req = "SELECT a.* FROM " + policy::ArtistTable::Name + " a "
                                         "INNER JOIN " + policy::AlbumTrackTable::Name + " att ON att.artist_id = a.id_artist "
                                         "WHERE att.genre_id = ? GROUP BY att.artist_id"
                                         " ORDER BY a.name";
  if (desc == true)
    req += " DESC";
  return Artist::fetchAll<IArtist>(m_ml, req, m_id);
}

std::vector<MediaPtr> Genre::tracks(SortingCriteria sort, bool desc) const {
  return AlbumTrack::fromGenre(m_ml, m_id, sort, desc);
}

std::vector<AlbumPtr> Genre::albums(SortingCriteria sort, bool desc) const {
  return Album::fromGenre(m_ml, m_id, sort, desc);
}
*/
bool mxp::Genre::CreateTable(DBConnection dbConn) {
  static const auto req = "CREATE TABLE IF NOT EXISTS " + GenreTable::Name +
      "("
      "id_genre INTEGER PRIMARY KEY AUTOINCREMENT,"
      "name TEXT UNIQUE ON CONFLICT FAIL"
      ")";
  static const auto vtableReq = "CREATE VIRTUAL TABLE IF NOT EXISTS "
      + GenreTable::Name + "Fts USING FTS3("
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
  return sqlite::Tools::ExecuteRequest(dbConn, req) &&
      sqlite::Tools::ExecuteRequest(dbConn, vtableReq) &&
      sqlite::Tools::ExecuteRequest(dbConn, vtableInsertTrigger) &&
      sqlite::Tools::ExecuteRequest(dbConn, vtableDeleteTrigger);
}

std::shared_ptr<mxp::Genre> mxp::Genre::Create(MediaExplorerPtr ml, const std::string& name) {
  static const auto req = "INSERT INTO " + GenreTable::Name + "(name) VALUES(?)";
  auto self = std::make_shared<Genre>(ml, name);
  if (insert(ml, self, req, name) == false)
    return nullptr;
  return self;
}

std::shared_ptr<mxp::Genre> mxp::Genre::FindByName(MediaExplorerPtr ml, const std::string& name) {
  static const auto req = "SELECT * FROM " + GenreTable::Name + " WHERE name = ?";
  return Fetch(ml, req, name);
}

std::vector<mxp::GenrePtr> mxp::Genre::Search(MediaExplorerPtr ml, const std::string& name) {
  static const auto req = "SELECT * FROM " + GenreTable::Name + " WHERE id_genre IN "
      "(SELECT rowid FROM " + GenreTable::Name + "Fts WHERE name MATCH ?)";
  return FetchAll<IGenre>(ml, req, name + "*");
}

std::vector<mxp::GenrePtr> mxp::Genre::ListAll(MediaExplorerPtr ml, SortingCriteria, bool desc) {
  auto req = "SELECT * FROM " + GenreTable::Name + " ORDER BY name";
  if (desc == true)
    req += " DESC";
  return FetchAll<IGenre>(ml, req);
}
