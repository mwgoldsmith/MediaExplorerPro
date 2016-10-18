/*****************************************************************************
* Media Explorer
*****************************************************************************/

#ifndef IMOVIE_H
#define IMOVIE_H


#include "mediaexplorer/Common.h"

namespace mxp {

class IMovie {
public:
  virtual ~IMovie() {}

  virtual int64_t Id() const = 0;

  virtual const std::string& GetTitle() const = 0;

  virtual const std::string& ShortSummary() const = 0;

  virtual const std::string& GetArtworkMrl() const = 0;

  virtual const std::string& imdbId() const = 0;

  virtual std::vector<MediaPtr> Files() = 0;
};

}

#endif // IMOVIE_H
