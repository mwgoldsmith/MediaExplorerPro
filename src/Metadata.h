/*****************************************************************************
* Media Explorer
*****************************************************************************/

#ifndef METADATA_H
#define METADATA_H

#include "database/DatabaseHelpers.h"
#include "mediaexplorer/IMetadata.h"
#include "mediaexplorer/Types.h"
#include "Types.h"

namespace mxp {

class Metadata;

namespace policy {
struct MetadataTable {
  static const std::string Name;
  static const std::string PrimaryKeyColumn;
  static int64_t Metadata::*const PrimaryKey;
};
}

class Metadata : public IMetadata, public DatabaseHelpers<Metadata, policy::MetadataTable> {
public:
  Metadata(MediaExplorerPtr ml, sqlite::Row& row);

public:
  virtual int64_t Id() const override;

  virtual const std::string& GetName() const override;

  virtual const std::string& GetValue() const override;

  virtual MediaPtr GetMedia() const override;

  virtual StreamPtr GetStream() const override;

  static bool CreateTable(DBConnection dbConnection);

private:
  MediaExplorerPtr m_ml;
  int64_t          m_id;

  friend struct policy::MetadataTable;
};

} /* namespace mxp */

#endif /* METADATA_H */
