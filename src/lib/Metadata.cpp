/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#include "stdafx.h"
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

bool mxp::Metadata::SetValue(const mstring& value) {
  static const auto req = "UPDATE " + MetadataTable::Name + " SET value=? WHERE " + MetadataTable::PrimaryKeyColumn + "=?";

  if(value != m_value) {
    if(sqlite::Tools::ExecuteUpdate(m_ml->GetConnection(), req, value, m_id) == false) {
      return false;
    }

    m_value = value;
  }

  return true;
}

mxp::MetadataPtr mxp::Metadata::Create(MediaExplorerPtr ml, const mstring& name, const mstring& value) noexcept {
  auto req = "INSERT INTO " + MetadataTable::Name + "(name, value, creation_date) VALUES(?, ?, ?)";
  std::shared_ptr<Metadata> self;

  try {
    self = std::make_shared<Metadata>(ml, name, value);
    if(insert(ml, self, req, self->m_name, self->m_value, self->m_creationDate) == false) {
      self = nullptr;
    }
  } catch(std::exception& ex) {
    LOG_ERROR(MTEXT("Failed to create Metadata: "), ex.what());
    self = nullptr;
  }

  return self;
}

bool mxp::Metadata::CreateTable(DBConnection connection) noexcept {
  static const auto req = "CREATE TABLE IF NOT EXISTS " + MetadataTable::Name + "(" +
    MetadataTable::PrimaryKeyColumn + " INTEGER PRIMARY KEY AUTOINCREMENT,"
    "name TEXT,"
    "value TEXT,"
    "creation_date UNSIGNED INT NOT NULL"
    ")";

  bool result;

  try {
    result = sqlite::Tools::ExecuteRequest(connection, req);
  } catch(std::exception& ex) {
    LOG_ERROR(MTEXT("Failed to create Metadata: "), ex.what());
    result = false;
  }

  return result;
}