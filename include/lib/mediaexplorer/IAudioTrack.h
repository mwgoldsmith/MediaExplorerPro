/*****************************************************************************
* Media Explorer
*****************************************************************************/

#ifndef MXP_IAUDIOTRACK_H
#define MXP_IAUDIOTRACK_H

#include "mediaexplorer/Common.h"

namespace mxp {

class IAudioTrack {
public:
  virtual ~IAudioTrack() {}

  /**
   * @return Identifier of the record for the audio track.
   */
  virtual int64_t Id() const = 0;

  virtual const std::string& Codec() const = 0;

  /**
   * @return The bitrate in bits per second
   */
  virtual unsigned int GetBitrate() const = 0;

  virtual unsigned int GetSampleRate() const = 0;

  virtual unsigned int GetNumChannels() const = 0;

  virtual const std::string& Language() const = 0;

  virtual const std::string& GetDescription() const = 0;
};

} /* namespace mxp */

#endif /* MXP_IAUDIOTRACK_H */
