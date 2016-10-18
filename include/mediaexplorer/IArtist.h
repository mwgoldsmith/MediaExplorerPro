/*****************************************************************************
* Media Explorer
*****************************************************************************/

#pragma once


#include "mediaexplorer/Common.h"
#include "mediaexplorer/SortingCriteria.h"

namespace mxp {

class IArtist {
public:
  virtual ~IArtist() = default;

  virtual int64_t Id() const = 0;

  virtual const std::string& GetName() const = 0;

  virtual const std::string& GetShortBio() const = 0;

  virtual std::vector<AlbumPtr> albums(SortingCriteria sort = SortingCriteria::Default, bool desc = false) const = 0;

  virtual std::vector<MediaPtr> media(SortingCriteria sort = SortingCriteria::Default, bool desc = false) const = 0;

  virtual const std::string& GetArtworkMrl() const = 0;

  virtual const std::string& musicBrainzId() const = 0;
};

}