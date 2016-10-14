/*****************************************************************************
* Media Explorer
*****************************************************************************/

#if HAVE_CONFIG_H
# include "config.h"
#endif

#include "Container.h"
#include "database/SqliteTools.h"

using mxp::policy::ContainerTable;
const mstring ContainerTable::Name = MTEXT("Container");
const mstring ContainerTable::PrimaryKeyColumn = MTEXT("id_container");
int64_t mxp::Container::* const ContainerTable::PrimaryKey = &mxp::Container::m_id;

mxp::Container::Container(MediaExplorerPtr ml, sqlite::Row& row)
  : m_ml(ml) {
  row >> m_id
    >> m_extensions
    >> m_name
    >> m_longName
    >> m_mimeType
    >> m_type
    >> m_isSupported
    >> m_creationDate;
}

mxp::Container::Container(MediaExplorerPtr ml, const mstring& name, const mstring& longName, const mstring& extensions, const mstring& mimeType, MediaType type)
  : m_ml(ml)
  , m_extensions(extensions)
  , m_name(name)
  , m_longName(longName)
  , m_mimeType(mimeType)
  , m_type(type)
  , m_isSupported(true)
  , m_creationDate(time(nullptr)) {
}

int64_t mxp::Container::Id() const noexcept {
  return m_id;
}

const mstring& mxp::Container::GetName() const noexcept {
  return m_name;
}

const mstring& mxp::Container::GetLongName() const noexcept {
  return m_longName;
}

const mstring& mxp::Container::GetExtensions() const noexcept {
  return m_extensions;
}

const mstring& mxp::Container::GetMimeType() const noexcept {
  return m_mimeType;
}

mxp::MediaType mxp::Container::GetType() const noexcept {
  return m_type;
}

bool mxp::Container::IsSupported() const noexcept {
  return m_isSupported;
}

bool mxp::Container::SetSupported(bool value) noexcept {
  static const auto req = "UPDATE " + ContainerTable::Name + " SET is_supported=? WHERE " + ContainerTable::PrimaryKeyColumn + "=?";
  auto result = true;

  // Only attempt to change if it is different
  if(value != m_isSupported) {
    try {
      result = sqlite::Tools::ExecuteUpdate(m_ml->GetConnection(), req, value, m_id);
      if(result != false) {
        m_isSupported = value;
      }
    } catch(std::exception& ex) {
      LOG_ERROR(MTEXT("Failed to set value for Container: "), ex.what());
      result = false;
    }
  }

  return result;
}

//********
// Static methods
mxp::ContainerPtr mxp::Container::Create(MediaExplorerPtr ml, const mstring& name, const mstring& longName, const mstring& extensions, const mstring& mimeType, MediaType type) noexcept {
  static const auto req = "INSERT INTO " + ContainerTable::Name + "(extensions, name, long_name, mime_type, type, is_supported, creation_date) VALUES(?, ?, ?, ?, ?, ?, ?)";
  std::shared_ptr<Container> self;

  try {
    self = std::make_shared<Container>(ml, name, longName, extensions, mimeType, type);
    if(insert(ml, self, req, self->m_extensions, self->m_name, self->m_longName, self->m_mimeType, self->m_type, self->m_isSupported, self->m_creationDate) == false) {
      self = nullptr;
    }
  } catch(std::exception& ex) {
    LOG_ERROR(MTEXT("Failed to create Container: "), ex.what());
    self = nullptr;
  }

  return self;
}

bool mxp::Container::CreateTable(DBConnection connection) noexcept {
  static const auto req = "CREATE TABLE IF NOT EXISTS " + ContainerTable::Name + "(" +
    ContainerTable::PrimaryKeyColumn + " INTEGER PRIMARY KEY AUTOINCREMENT,"
    "extensions TEXT,"
    "name TEXT UNIQUE ON CONFLICT FAIL,"
    "long_name TEXT,"
    "mime_type TEXT,"
    "type INT,"
    "is_supported BOOLEAN NOT NULL DEFAULT 0,"
    "creation_date UNSIGNED INT NOT NULL"
    ")";

  bool result;

  try {
    result = sqlite::Tools::ExecuteRequest(connection, req);
  } catch(std::exception& ex) {
    LOG_ERROR(MTEXT("Failed to create table for "), ContainerTable::Name, MTEXT(": "), ex.what());
    result = false;
  }

  return result;
}

std::vector<mxp::ContainerPtr> mxp::Container::ListAll(MediaExplorerPtr ml, SortingCriteria sort, bool desc) {
  auto req = "SELECT * FROM " + ContainerTable::Name + " ORDER BY name";
  if(desc == true) {
    req += " DESC";
  }

  return FetchAll<IContainer>(ml, req);
}
