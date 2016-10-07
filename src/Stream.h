/*****************************************************************************
* Media Explorer
*****************************************************************************/

#ifndef STREAM_H
#define STREAM_H

#include "database/DatabaseHelpers.h"
#include "mediaexplorer/IStream.h"
#include "mediaexplorer/Types.h"
#include "Types.h"

namespace mxp {

class Stream;

namespace policy {
struct StreamTable {
  static const std::string Name;
  static const std::string PrimaryKeyColumn;
  static int64_t Stream::*const PrimaryKey;
};
}

class Stream : public IStream, public DatabaseHelpers<Stream, policy::StreamTable> {
public:
  Stream(MediaExplorerPtr ml, sqlite::Row& row);

public:
  virtual int64_t Id() const override;

  virtual uint32_t GetIndex() const override;

  virtual CodecPtr GetCodec() const override;

  virtual MediaType GetType() const override;

  virtual MediaPtr GetMedia() const override;

  virtual std::vector<MetadataPtr> GetMetadata() const override;

  static bool CreateTable(DBConnection dbConnection);

private:
  MediaExplorerPtr m_ml;
  int64_t          m_id;

  friend struct policy::StreamTable;
};

} /* namespace mxp */


#endif /* STREAM_H */
