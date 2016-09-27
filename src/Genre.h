﻿/*****************************************************************************
* Media Explorer
*****************************************************************************/

#ifndef GENRE_H
#define GENRE_H

#include "database/DatabaseHelpers.h"
#include "mediaexplorer/IGenre.h"

namespace mxp {

class Genre;

namespace policy {
struct GenreTable {
  static const std::string Name;
  static const std::string PrimaryKeyColumn;
  static int64_t Genre::*const PrimaryKey;
};
}

class Genre : public IGenre, public DatabaseHelpers<Genre, policy::GenreTable> {
public:
  Genre(MediaExplorerPtr ml, sqlite::Row& row);
  Genre(MediaExplorerPtr ml, const std::string& name);
  virtual int64_t id() const override;
  virtual const std::string& name() const override;
  //virtual std::vector<ArtistPtr> artists(SortingCriteria sort, bool desc) const override;
  //virtual std::vector<MediaPtr> tracks(SortingCriteria sort, bool desc) const override;
  //virtual std::vector<AlbumPtr> albums(SortingCriteria sort, bool desc) const override;

  static bool CreateTable(DBConnection dbConn);
  static std::shared_ptr<Genre> create(MediaExplorerPtr ml, const std::string& name);
  static std::shared_ptr<Genre> fromName(MediaExplorerPtr ml, const std::string& name);
  static std::vector<GenrePtr> search(MediaExplorerPtr ml, const std::string& name);
  static std::vector<GenrePtr> listAll(MediaExplorerPtr ml, SortingCriteria sort, bool desc);

private:
  MediaExplorerPtr m_ml;

  int64_t m_id;
  std::string m_name;

  friend policy::GenreTable;
};

} /* namespace mxp */

#endif /* GENRE_H */