/*****************************************************************************
* Media Explorer
*****************************************************************************/

#ifndef MXP_IGENRE_H
#define MXP_IGENRE_H

#include "mediaexplorer/Common.h"

namespace mxp {

class IGenre {
public:
  virtual ~IGenre() = default;

  /**
   * @return Identifier of the record for the genre
   */
  virtual int64_t Id() const = 0;

  /**
   * @return Name of the genre
   */
  virtual const std::string& Name() const = 0;

  virtual std::vector<ArtistPtr> Artists(SortingCriteria sort = SortingCriteria::Default, bool desc = false) const = 0;

  virtual std::vector<MediaPtr> Tracks(SortingCriteria sort = SortingCriteria::Default, bool desc = false) const = 0;

  virtual std::vector<AlbumPtr> Albums(SortingCriteria sort = SortingCriteria::Default, bool desc = false) const = 0;
};

} /* namespace mxp */

#endif /* MXP_IGENRE_H */
