/*****************************************************************************
* Media Explorer
*****************************************************************************/

#pragma once

#include "mediaexplorer/Types.h"
#include "mediaexplorer/SortingCriteria.h"

namespace mxp {

class IArtist {
public:
  virtual ~IArtist() = default;

  virtual int64_t Id() const = 0;

  virtual const std::string& name() const = 0;

  virtual const std::string& shortBio() const = 0;

  virtual std::vector<AlbumPtr> albums(SortingCriteria sort = SortingCriteria::Default, bool desc = false) const = 0;

  virtual std::vector<MediaPtr> media(SortingCriteria sort = SortingCriteria::Default, bool desc = false) const = 0;

  virtual const std::string& artworkMrl() const = 0;

  virtual const std::string& musicBrainzId() const = 0;
};

}