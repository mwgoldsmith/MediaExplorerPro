/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#ifndef IVIDEOTRACK_H
#define IVIDEOTRACK_H

#include "mediaexplorer/IMediaExplorer.h"

namespace mxp {

class IVideoTrack {
public:
  virtual ~IVideoTrack() {}

  virtual int64_t id() const = 0;
  virtual const std::string& codec() const = 0;
  virtual unsigned int width() const = 0;
  virtual unsigned int height() const = 0;
  virtual float fps() const = 0;
  virtual const std::string& description() const = 0;
  virtual const std::string& language() const = 0;
};

}

#endif // IVIDEOTRACK_H

