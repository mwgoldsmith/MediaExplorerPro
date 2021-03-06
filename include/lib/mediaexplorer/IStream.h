/*****************************************************************************
* Media Explorer
*****************************************************************************/

#ifndef MXP_ISTREAM_H
#define MXP_ISTREAM_H

#include "mediaexplorer/Common.h"

namespace mxp {

class IStream {
public:
  virtual ~IStream() = default;

  /**
   * @return Identifier of the record for the stream.
   */
  virtual int64_t Id() const = 0;

  /**
   * @return 
   */
  virtual uint32_t GetIndex() const = 0;

  /**
   * @return
   */
  virtual CodecPtr GetCodec() const = 0;

  /**
   * @return
   */
  virtual MediaType GetType() const = 0;

  /**
   * @return Pointer to the instance of the Media record the stream is associated with.
   */
  virtual MediaPtr GetMedia() const = 0;

  /**
  * @param metadataId
  *
  * @return
  */
  virtual bool AddMetadata(int64_t metadataId) = 0;

  /**
  * @param metadataId
  *
  * @return
  */
  virtual bool RemoveMetadata(int64_t metadataId) = 0;

  /**
   * @return
   */
  virtual const std::vector<MetadataPtr>& GetMetadata() const = 0;
};

} /* namespace mxp */

#endif /* MXP_ISTREAM_H */
