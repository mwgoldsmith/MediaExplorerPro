﻿/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#ifndef MXP_IVIDEOTRACK_H
#define MXP_IVIDEOTRACK_H

#include "mediaexplorer/Common.h"

namespace mxp {

class IVideoTrack {
public:
  virtual ~IVideoTrack() {}

  /**
   * @return Identifier of the record for the video track.
   */
  virtual int64_t Id() const = 0;

  virtual const std::string& Codec() const = 0;

  virtual unsigned int Width() const = 0;

  virtual unsigned int Height() const = 0;

  virtual float Fps() const = 0;

  virtual const std::string& GetDescription() const = 0;

  virtual const std::string& Language() const = 0;
};

} /* namespace mxp */

#endif /* MXP_IVIDEOTRACK_H */
