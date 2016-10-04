/*****************************************************************************
* Media Explorer
*****************************************************************************/

#ifndef MXP_IGENRE_H
#define MXP_IGENRE_H

#include <stdint.h>
#include <string>
#include <vector>

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

  //virtual std::vector<ArtistPtr> artists(SortingCriteria sort = SortingCriteria::Default, bool desc = false) const = 0;

  //virtual std::vector<MediaPtr> tracks(SortingCriteria sort = SortingCriteria::Default, bool desc = false) const = 0;

  //virtual std::vector<AlbumPtr> albums(SortingCriteria sort = SortingCriteria::Default, bool desc = false) const = 0;
};

}

#endif /* MXP_IGENRE_H */
