/*****************************************************************************
* Media Explorer
*****************************************************************************/

#ifndef METADATA_H
#define METADATA_H

#include "database/DatabaseHelpers.h"
#include "mediaexplorer/IMetadata.h"
#include "mediaexplorer/Common.h"
#include "Types.h"

namespace mxp {

class Metadata;

namespace policy {
struct MetadataTable {
  static const mstring Name;
  static const mstring PrimaryKeyColumn;
  static int64_t Metadata::*const PrimaryKey;
};
}

class Metadata : public IMetadata, public DatabaseHelpers<Metadata, policy::MetadataTable> {
public:
  Metadata(MediaExplorerPtr ml, sqlite::Row& row);

  Metadata(MediaExplorerPtr ml, const mstring& name, const mstring& value);

  virtual int64_t Id() const override;

  virtual const mstring& GetName() const override;

  virtual const mstring& GetValue() const override;

  virtual bool SetValue(const mstring& value) override;

  static MetadataPtr Create(MediaExplorerPtr ml, const mstring& name, const mstring& value) noexcept;

  static bool CreateTable(DBConnection dbConnection) noexcept;

private:
  MediaExplorerPtr m_ml;
  int64_t          m_id;
  mstring      m_name;
  mstring      m_value;
  time_t           m_creationDate;

  friend struct policy::MetadataTable;
};

} /* namespace mxp */

#endif /* METADATA_H */
