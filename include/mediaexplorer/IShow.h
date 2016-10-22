/*****************************************************************************
* Media Explorer
*****************************************************************************/

#ifndef MXP_ISHOW_H
#define MXP_ISHOW_H

#include "mediaexplorer/Common.h"

namespace mxp {

class IShow {
public:
  virtual ~IShow() {}

  virtual int64_t Id() const = 0;

  virtual const std::string& GetName() const = 0;

  virtual time_t GetReleaseDate() const = 0;

  virtual const std::string& GetShortSummary() const = 0;

  virtual const std::string& GetArtworkMrl() const = 0;

  virtual const std::string& GetTvdbId() = 0;

  virtual std::vector<ShowEpisodePtr> GetEpisodes() = 0;
};

} /* namespace mxp */

#endif /* MXP_ISHOW_H */
