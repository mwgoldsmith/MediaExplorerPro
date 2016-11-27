/*****************************************************************************
* Media Explorer
*****************************************************************************/

#ifndef MXP_IALBUMTRACK_H
#define MXP_IALBUMTRACK_H

#include "mediaexplorer/Common.h"

namespace mxp {

class IAlbumTrack {
public:
  virtual ~IAlbumTrack() = default;

  virtual int64_t Id() const = 0;

  /**
   * @brief artist Returns the artist, as tagged in the media.
   * This can be different from the associated media's artist.
   * For instance, in case of a featuring, Media::artist() might return
   * "Artist 1", while IAlbumTrack::artist() might return something like
   * "Artist 1 featuring Artist 2 and also artist 3 and a whole bunch of people"
   * @return
   */
  virtual ArtistPtr GetArtist() const = 0;

  virtual GenrePtr GetGenre() = 0;

  virtual unsigned int GetTrackNumber() = 0;

  virtual AlbumPtr GetAlbum() = 0;

  virtual MediaPtr GetMedia() = 0;

  /**
   * @return Which disc this tracks appears on (or 0 if unspecified)
   */
  virtual unsigned int GetDiscNumber() const = 0;
};

} /* namespace mxp */

#endif /* MXP_IALBUMTRACK_H */
