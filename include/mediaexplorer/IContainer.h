/*****************************************************************************
* Media Explorer
*****************************************************************************/

#ifndef MXP_ICONTAINER_H
#define MXP_ICONTAINER_H

#include <cinttypes>
#include <string>
#include "mediaexplorer/Types.h"

namespace mxp {
enum class MediaType;

class IContainer {
public:
  virtual ~IContainer() = default;

  /**
  * @return Identifier of the record for the container.
  */
  virtual int64_t Id() const noexcept = 0;

  /**
  * @return The name of the container.
  */
  virtual const mstring& GetName() const noexcept = 0;

  /**
  * @return The long name of the container.
  */
  virtual const mstring& GetLongName() const noexcept = 0;

  /**
  * @return The extensions of the container.
  */
  virtual const mstring& GetExtensions() const noexcept = 0;

  /**
  * @return The mime type of the container.
  */
  virtual const mstring& GetMimeType() const noexcept = 0;

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
