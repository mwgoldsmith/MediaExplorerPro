/*****************************************************************************
* Media Explorer
*****************************************************************************/

#ifndef MXP_IALBUM_H
#define MXP_IALBUM_H

#include "mediaexplorer/Common.h"

namespace mxp {

class IAlbum {
public:
  virtual ~IAlbum() = default;

  virtual int64_t Id() const = 0;

  virtual const std::string& GetTitle() const = 0;

  /**
   * @brief releaseYear returns the release year, or 0 if unknown.
   */
  virtual unsigned int ReleaseYear() const = 0;

  virtual const std::string& ShortSummary() const = 0;

  virtual const std::string& GetArtworkMrl() const = 0;

  /**
   * @brief tracks fetches album tracks from the database
   */
  virtual std::vector<MediaPtr> Tracks(SortingCriteria sort = SortingCriteria::Default, bool desc = false) const = 0;

  /**
   * @brief tracks fetches album tracks, filtered by genre
   * @param genre A musical genre. Only tracks of this genre will be returned
   * @return
   */
  virtual std::vector<MediaPtr> Tracks(GenrePtr genre, SortingCriteria sort = SortingCriteria::Default, bool desc = false) const = 0;

  /**
   * @brief albumArtist Returns the album main artist (generally tagged as album-artist)
   */
  virtual ArtistPtr AlbumArtist() const = 0;

  /**
   * @brief artists Returns a vector of all additional artists appearing on the album.
   * Artists are sorted by name.
   * @param desc
   */
  virtual std::vector<ArtistPtr> Artists(bool desc) const = 0;

  /**
   * @brief nbTracks Returns the amount of track in this album.
   * The value is cached, and doesn't require fetching anything.
   */
  virtual uint32_t GetNumTracks() const = 0;

  virtual unsigned int Duration() const = 0;
};

} /* namespace mxp */

#endif /* MXP_IALBUM_H */
