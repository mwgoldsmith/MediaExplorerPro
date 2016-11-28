/*****************************************************************************
* Media Explorer
*****************************************************************************/

#ifndef MXP_ICODEC_H
#define MXP_ICODEC_H

#include "mediaexplorer/Common.h"

namespace mxp {

class ICodec {
public:
  virtual ~ICodec() = default;

  /**
  * @return Identifier of the record for the codec.
  */
  virtual int64_t Id() const noexcept = 0;

  /**
  * @return The name of the codec.
  */
  virtual const mstring& GetName() const noexcept = 0;

  /**
  * @return The more descriptive name of the codec.
  */
  virtual const mstring& GetLongName() const noexcept = 0;

  /**
  * @return
  */
  virtual MediaType GetType() const noexcept = 0;

  /**
  * @return
  */
  virtual bool IsSupported() const noexcept = 0;

  /**
  * @param value
  */
  virtual bool SetSupported(bool value) noexcept = 0;
};

} /* namespace mxp */

#endif /* MXP_ICODEC_H */
