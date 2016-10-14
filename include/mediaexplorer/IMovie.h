/*****************************************************************************
* Media Explorer
*****************************************************************************/

#ifndef IMOVIE_H
#define IMOVIE_H

#include "mediaexplorer/Types.h"

namespace mxp {

class IMovie {
public:
  virtual ~IMovie() {}

  virtual int64_t Id() const = 0;

  virtual const std::string& Title() const = 0;

  virtual const std::string& ShortSummary() const = 0;

  virtual const std::string& artworkMrl() const = 0;

  virtual const std::string& imdbId() const = 0;

  virtual std::vector<MediaPtr> Files() = 0;
};

}

#endif // IMOVIE_H
