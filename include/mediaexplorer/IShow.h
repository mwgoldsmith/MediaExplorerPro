/*****************************************************************************
* Media Explorer
*****************************************************************************/

#ifndef ISHOW_H
#define ISHOW_H

#include <cinttypes>
#include <string>
#include <vector>
#include "mediaexplorer/Types.h"

namespace mxp {

class IShow {
public:
  virtual ~IShow() {}

  virtual int64_t Id() const = 0;

  virtual const std::string& name() const = 0;

  virtual time_t releaseDate() const = 0;

  virtual const std::string& shortSummary() const = 0;

  virtual const std::string& artworkMrl() const = 0;

  virtual const std::string& tvdbId() = 0;

  virtual std::vector<ShowEpisodePtr> episodes() = 0;
};

}

#endif // ISHOW_H
