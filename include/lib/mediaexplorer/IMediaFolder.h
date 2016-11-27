/*****************************************************************************
* Media Explorer
*****************************************************************************/

#ifndef MXP_IMEDIAFOLDER_H
#define MXP_IMEDIAFOLDER_H

#include <string>

namespace mxp {

class IMediaFolder {
public:
  virtual ~IMediaFolder() = default;

  virtual int64_t id() const = 0;
  virtual const std::string& path() const = 0;
  virtual int64_t deviceId() const = 0;
  virtual bool isPresent() const = 0;
};

} /* namespace mxp */

#endif /* MXP_IMEDIAFOLDER_H */
