/*****************************************************************************
* Media Explorer
*****************************************************************************/

#ifndef MXP_IMOVIE_H
#define MXP_IMOVIE_H

#include "mediaexplorer/Common.h"

namespace mxp {

class IMovie {
public:
  virtual ~IMovie() {}

  virtual int64_t Id() const = 0;

  virtual const mstring& GetTitle() const = 0;

  virtual const mstring& GetShortSummary() const = 0;

  virtual const mstring& GetArtworkMrl() const = 0;

  virtual const mstring& GetImdbId() const = 0;

  virtual std::vector<MediaPtr> GetMedia() = 0;
};

} /* namespace mxp */

#endif /* MXP_IMOVIE_H */
