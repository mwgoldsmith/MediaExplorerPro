/*****************************************************************************
* Media Explorer
*****************************************************************************/

#ifndef ISHOWEPISODE_H
#define ISHOWEPISODE_H

#include <cinttypes>
#include <memory>
#include <vector>
#include "mediaexplorer/Types.h"

namespace mxp {

class IShowEpisode {
public:
  virtual ~IShowEpisode() {}

  virtual int64_t Id() const = 0;

  virtual const std::string& artworkMrl() const = 0;

  virtual unsigned int episodeNumber() const = 0;

  virtual const std::string& name() const = 0;

  virtual unsigned int seasonNumber() const = 0;

  virtual const std::string& shortSummary() const = 0;

  virtual const std::string& tvdbId() const = 0;

  virtual ShowPtr show() = 0;

  virtual std::vector<MediaPtr> files() = 0;
};

}

#endif // ISHOWEPISODE_H
