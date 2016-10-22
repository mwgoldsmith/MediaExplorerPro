/*****************************************************************************
* Media Explorer
*****************************************************************************/

#ifndef MXP_ISHOWEPISODE_H
#define MXP_ISHOWEPISODE_H

#include "mediaexplorer/Common.h"

namespace mxp {

class IShowEpisode {
public:
  virtual ~IShowEpisode() {}

  virtual int64_t Id() const = 0;

  virtual const std::string& GetArtworkMrl() const = 0;

  virtual unsigned int episodeNumber() const = 0;

  virtual const std::string& GetName() const = 0;

  virtual unsigned int seasonNumber() const = 0;

  virtual const std::string& GetShortSummary() const = 0;

  virtual const std::string& GetTvdbId() const = 0;

  virtual ShowPtr show() = 0;

  virtual std::vector<MediaPtr> files() = 0;
};

} /* namespace mxp */

#endif /* MXP_ISHOWEPISODE_H */
