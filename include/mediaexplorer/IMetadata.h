/*****************************************************************************
* Media Explorer
*****************************************************************************/

#ifndef MXP_IMETADATA_H
#define MXP_IMETADATA_H

#include <cinttypes>
#include <string>
#include "mediaexplorer/Types.h"

namespace mxp {

class IMetadata {
public:
  virtual ~IMetadata() = default;

  /**
   * @return Identifier of the record for the metadata.
   */
  virtual int64_t Id() const = 0;

  /**
   * @return The name of the metadata entry.
   */
  virtual const std::string& GetName() const = 0;

  /**
   * @return The value of the metadata entry.
   */
  virtual const std::string& GetValue() const = 0;

  /**
   * @return Pointer to the instance of the Media record the metadata is associated with.
   */
  virtual MediaPtr GetMedia() const = 0;

  /**
   * @return Pointer to the instance of the Stream record the metadata is associated with,
   *         or nullptr if not associated with a Stream.
   */
  virtual StreamPtr GetStream() const = 0;
};

} /* namespace mxp */

#endif /* MXP_IMETADATA_H */
