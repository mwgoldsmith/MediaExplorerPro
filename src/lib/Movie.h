/*****************************************************************************
* Media Explorer
*****************************************************************************/

#ifndef MOVIE_H
#define MOVIE_H

#include "mediaexplorer/IMovie.h"
#include "database/DatabaseHelpers.h"
#include "mediaexplorer/Common.h"
#include "Types.h"

namespace mxp {

namespace policy {
struct MovieTable {
  static const mstring Name;
  static const mstring PrimaryKeyColumn;
  static int64_t Movie::*const PrimaryKey;
};
}

class Movie : public IMovie, public DatabaseHelpers<Movie, policy::MovieTable> {
public:
  Movie(MediaExplorerPtr ml, sqlite::Row& row);

  Movie(MediaExplorerPtr ml, int64_t mediaId, const mstring& title);

  virtual int64_t Id() const override;

  virtual const mstring& GetTitle() const override;

  virtual const mstring& GetShortSummary() const override;

  virtual const mstring& GetArtworkMrl() const override;

  virtual const mstring& GetImdbId() const override;

  virtual std::vector<MediaPtr> GetMedia() override;

  bool SetShortSummary(const mstring& summary);

  bool SetArtworkMrl(const mstring& artworkMrl);

  bool SetImdbId(const mstring& imdbId);


  static bool CreateTable(DBConnection dbConnection);

  static std::shared_ptr<Movie> Create(MediaExplorerPtr ml, int64_t mediaId, const mstring& title);

  static MoviePtr FromMedia(MediaExplorerPtr ml, int64_t mediaId);

private:
  MediaExplorerPtr m_ml;
  int64_t          m_id;
  int64_t          m_mediaId;
  mstring          m_title;
  mstring          m_summary;
  mstring          m_artworkMrl;
  mstring          m_imdbId;

  friend struct policy::MovieTable;
};

} /* namespace mxp */

#endif /* MOVIE_H */
