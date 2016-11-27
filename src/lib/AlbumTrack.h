/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#ifndef ALBUMTRACK_H
#define ALBUMTRACK_H

#include "mediaexplorer/IAlbumTrack.h"
#include "database/DatabaseHelpers.h"
#include "utils/Cache.h"
#include "mediaexplorer/IMediaExplorer.h"
#include "mediaexplorer/Common.h"
#include "Types.h"

namespace mxp {

namespace policy {
struct AlbumTrackTable {
  static const std::string Name;
  static const std::string PrimaryKeyColumn;
  static int64_t AlbumTrack::*const PrimaryKey;
};
}

class AlbumTrack : public IAlbumTrack, public DatabaseHelpers<AlbumTrack, policy::AlbumTrackTable> {
public:
  AlbumTrack(MediaExplorerPtr ml, sqlite::Row& row);

  AlbumTrack(MediaExplorerPtr ml, int64_t mediaId, unsigned int trackNumber, int64_t albumId, unsigned int discNumber);

  virtual int64_t Id() const override;

  virtual ArtistPtr GetArtist() const override;

  virtual GenrePtr GetGenre() override;

  virtual unsigned int GetTrackNumber() override;

  virtual unsigned int GetDiscNumber() const override;

  virtual std::shared_ptr<IAlbum> GetAlbum() override;

  virtual std::shared_ptr<IMedia> GetMedia() override;

  bool SetArtist(std::shared_ptr<Artist> artist);

  bool SetGenre(std::shared_ptr<Genre> genre);

  static bool CreateTable(DBConnection dbConnection);

  static std::shared_ptr<AlbumTrack> Create(MediaExplorerPtr ml, int64_t albumId, std::shared_ptr<Media> media, unsigned int trackNb, unsigned int discNumber);

  static AlbumTrackPtr FindByMedia(MediaExplorerPtr ml, int64_t mediaId);

  static std::vector<MediaPtr> FindByGenre(MediaExplorerPtr ml, int64_t genreId, SortingCriteria sort, bool desc);

private:
  MediaExplorerPtr m_ml;
  int64_t          m_id;
  int64_t          m_mediaId;
  int64_t          m_artistId;
  int64_t          m_genreId;
  unsigned int     m_trackNumber;
  int64_t          m_albumId;
  unsigned int     m_discNumber;
  bool             m_isPresent;
  mutable Cache<std::weak_ptr<Album>>    m_album;
  mutable Cache<std::shared_ptr<Artist>> m_artist;
  mutable Cache<std::shared_ptr<Genre>>  m_genre;
  mutable Cache<std::weak_ptr<Media>>    m_media;

  friend struct policy::AlbumTrackTable;
};

} /* namespace mxp */

#endif /* ALBUMTRACK_H */
