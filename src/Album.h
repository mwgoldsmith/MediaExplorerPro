/*****************************************************************************
* Media Explorer
*****************************************************************************/

#ifndef ALBUM_H
#define ALBUM_H

#include "database/DatabaseHelpers.h"
#include "mediaexplorer/IAlbum.h"
#include "utils/Cache.h"
#include "mediaexplorer/Common.h"
#include "Types.h"

namespace mxp {

namespace policy {
struct AlbumTable {
  static const std::string Name;
  static const std::string PrimaryKeyColumn;
  static int64_t Album::*const PrimaryKey;
};
}

class Album : public IAlbum, public DatabaseHelpers<Album, policy::AlbumTable> {
public:
  Album(MediaExplorerPtr ml, sqlite::Row& row);
  Album(MediaExplorerPtr ml, const std::string& title);
  Album(MediaExplorerPtr ml, const Artist* artist);

  virtual int64_t Id() const override;
  virtual const std::string& GetTitle() const override;
  virtual unsigned int ReleaseYear() const override;
  /**
   * @brief setReleaseYear Updates the release year
   * @param date The desired date.
   * @param force If force is true, the date will be applied no matter what.
   *              If force is false, the date will be applied if it never was
   *              applied before. Otherwise, setReleaseYear() will restore the release
   *              date to 0.
   * @return
   */
  bool SetReleaseYear(unsigned int date, bool force);
  virtual const std::string& ShortSummary() const override;
  bool SetShortSummary(const std::string& summary);
  virtual const std::string& GetArtworkMrl() const override;
  bool SetArtworkMrl(const std::string& artworkMrl);
  virtual std::vector<MediaPtr> Tracks(SortingCriteria sort, bool desc) const override;
  virtual std::vector<MediaPtr> Tracks(GenrePtr genre, SortingCriteria sort, bool desc) const override;
  ///
  /// \brief cachedTracks Returns a cached list of tracks
  /// This has no warranty of ordering, validity, or anything else.
  /// \return An unordered-list of this album's tracks
  ///
  std::vector<MediaPtr> CachedTracks() const;
  ///
  /// \brief addTrack Add a track to the album.
  /// This will modify the media, but *not* save it.
  /// The media will be added to the tracks cache.
  ///
  std::shared_ptr<AlbumTrack> AddTrack(std::shared_ptr<Media> media, unsigned int trackNb, unsigned int discNumber);
  unsigned int GetNumTracks() const override;
  unsigned int Duration() const override;

  virtual ArtistPtr AlbumArtist() const override;
  bool SetAlbumArtist(std::shared_ptr<Artist> artist);
  virtual std::vector<ArtistPtr> Artists(bool desc) const override;
  bool AddArtist(std::shared_ptr<Artist> artist);
  bool RemoveArtist(Artist* artist);

  static bool CreateTable(DBConnection dbConnection);
  static bool CreateTriggers(DBConnection dbConnection);
  static std::shared_ptr<Album> Create(MediaExplorerPtr ml, const std::string& title);
  static std::shared_ptr<Album> CreateUnknownAlbum(MediaExplorerPtr ml, const Artist* artist);
  ///
  /// \brief search search for an album, through its albumartist or title
  /// \param pattern A pattern representing the title, or the name of the main artist
  /// \return
  ///
  static std::vector<AlbumPtr> Search(MediaExplorerPtr ml, const std::string& pattern);
  static std::vector<AlbumPtr> FromArtist(MediaExplorerPtr ml, int64_t artistId, SortingCriteria sort, bool desc);
  static std::vector<AlbumPtr> FindByGenre(MediaExplorerPtr ml, int64_t genreId, SortingCriteria sort, bool desc);
  static std::vector<AlbumPtr> ListAll(MediaExplorerPtr ml, SortingCriteria sort, bool desc);

private:
  static std::string OrderTracksBy(SortingCriteria sort, bool desc);
  static std::string OrderBy(SortingCriteria sort, bool desc);

protected:
  MediaExplorerPtr     m_ml;
  int64_t              m_id;
  std::string          m_title;
  int64_t              m_artistId;
  unsigned int         m_releaseYear;
  std::string          m_shortSummary;
  std::string          m_artworkMrl;
  unsigned int         m_nbTracks;
  int64_t              m_duration;
  bool                 m_isPresent;

  mutable Cache<std::vector<MediaPtr>>   m_tracks;
  mutable Cache<std::shared_ptr<Artist>> m_albumArtist;

  friend struct policy::AlbumTable;
};

}

#endif // ALBUM_H
