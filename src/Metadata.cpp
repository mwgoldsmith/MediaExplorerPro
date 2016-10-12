/*****************************************************************************
* Media Explorer
*****************************************************************************/

#if HAVE_CONFIG_H
# include "config.h"
#endif

#include "Metadata.h"
#include "database/SqliteTools.h"

using mxp::policy::MetadataTable;
const mstring MetadataTable::Name = "Metadata";
const mstring MetadataTable::PrimaryKeyColumn = "id_metadata";
int64_t mxp::Metadata::* const MetadataTable::PrimaryKey = &mxp::Metadata::m_id;

mxp::Metadata::Metadata(MediaExplorerPtr ml, sqlite::Row& row)
  : m_ml(ml) {
  row >> m_id
      >> m_name
      >> m_value
      >> m_creationDate;
}

mxp::Metadata::Metadata(MediaExplorerPtr ml, const mstring& name, const mstring& value)
  : m_ml(ml)
  , m_id(0)
  , m_name(name)
  , m_value(value) 
  , m_creationDate(time(nullptr)) {
}

int64_t mxp::Metadata::Id() const {
  return m_id;
}

const mstring& mxp::Metadata::GetName() const {
  return m_name;
}

const mstring& mxp::Metadata::GetValue() const {
  return m_value;
}

mxp::MetadataPtr mxp::Metadata::Create(MediaExplorerPtr ml, const mstring& name, const mstring& value) {
  auto self = std::make_shared<Metadata>(ml, name, value);
  auto req = "INSERT INTO " + MetadataTable::Name + "(name, value, creation_date) VALUES(?, ?, ?)";

  if(insert(ml, self, req, self->m_name, self->m_value, self->m_creationDate) == false) {
    return nullptr;
  }

  return self;
}

bool mxp::Metadata::CreateTable(DBConnection dbConnection) {
  static const auto req = "CREATE TABLE IF NOT EXISTS " + MetadataTable::Name + "(" +
    MetadataTable::PrimaryKeyColumn + " INTEGER PRIMARY KEY AUTOINCREMENT,"
    "name TEXT,"
    "value TEXT,"
    "creation_date UNSIGNED INT NOT NULL"
    ")";

  return sqlite::Tools::ExecuteRequest(dbConnection, req);
}