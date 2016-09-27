/*****************************************************************************
* Media Explorer
*****************************************************************************/

#ifndef MXP_IGENRE_H
#define MXP_IGENRE_H

#include <vector>
#include "IMediaExplorer.h"

namespace mxp {

class IGenre {
public:
  virtual ~IGenre() = default;
  virtual int64_t id() const = 0;
  virtual const std::string& name() const = 0;
  //virtual std::vector<ArtistPtr> artists(SortingCriteria sort = SortingCriteria::Default, bool desc = false) const = 0;
  //virtual std::vector<MediaPtr> tracks(SortingCriteria sort = SortingCriteria::Default, bool desc = false) const = 0;
  //virtual std::vector<AlbumPtr> albums(SortingCriteria sort = SortingCriteria::Default, bool desc = false) const = 0;
};

}

#endif /* MXP_IGENRE_H */
