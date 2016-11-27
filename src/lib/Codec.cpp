/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#include "stdafx.h"
#if HAVE_CONFIG_H
# include "config.h"
#endif

#include "Codec.h"
#include "database/SqliteTools.h"

using mxp::policy::CodecTable;
const mstring CodecTable::Name = MTEXT("Codec");
const mstring CodecTable::PrimaryKeyColumn = MTEXT("id_codec");
int64_t mxp::Codec::* const CodecTable::PrimaryKey = &mxp::Codec::m_id;

mxp::Codec::Codec(MediaExplorerPtr ml, sqlite::Row& row)
  : m_ml(ml) {
  row >> m_id
    >> m_name
    >> m_longName
    >> m_type
    >> m_isSupported
    >> m_creationDate;
}

mxp::Codec::Codec(MediaExplorerPtr ml, const mstring& name, const mstring& longName, MediaType type)
  : m_ml(ml)
  , m_id(0)
  , m_name(name)
  , m_longName(longName)
  , m_type(type)
  , m_isSupported(true)
  , m_creationDate(time(nullptr)) {
}

int64_t mxp::Codec::Id() const noexcept {
  return m_id;
}

const mstring& mxp::Codec::GetName() const noexcept {
  return m_name;
}

const mstring& mxp::Codec::GetLongName() const noexcept {
  return m_longName;
}

mxp::MediaType mxp::Codec::GetType() const noexcept {
  return m_type;
}

bool mxp::Codec::IsSupported() const noexcept {
  return m_isSupported;
}

bool mxp::Codec::SetSupported(bool value) noexcept {
  static const auto req = "UPDATE " + CodecTable::Name + " SET is_supported=? WHERE " + CodecTable::PrimaryKeyColumn + "=?";
  auto result = true;

  // Only attempt to change if it is different
  if(value != m_isSupported) {
    try {
      result = sqlite::Tools::ExecuteUpdate(m_ml->GetConnection(), req, value, m_id);
      if(result != false) {
        m_isSupported = value;
      }
    } catch(std::exception& ex) {
      LOG_ERROR(MTEXT("Failed to set value for Codec: "), ex.what());
      result = false;
    }
  }

  return result;
}

//********
// Static methods
mxp::CodecPtr mxp::Codec::Create(MediaExplorerPtr ml, const mstring& name, const mstring& longName, MediaType type) noexcept {
  static const auto req = "INSERT INTO " + CodecTable::Name + "(name, long_name, type, is_supported, creation_date) VALUES(?, ?, ?, ?, ?)";
  std::shared_ptr<Codec> self;

  try {
    self = std::make_shared<Codec>(ml, name, longName, type);
    if(insert(ml, self, req, self->m_name, self->m_longName, self->m_type, self->m_isSupported, self->m_creationDate) == false) {
      self = nullptr;
    }
  } catch(std::exception& ex) {
    LOG_ERROR(MTEXT("Failed to create Codec: "), ex.what());
    self = nullptr;
  }

  return self;
}

bool mxp::Codec::CreateTable(DBConnection dbConn) noexcept {
  static const auto req = "CREATE TABLE IF NOT EXISTS " + CodecTable::Name + "(" +
    CodecTable::PrimaryKeyColumn + " INTEGER PRIMARY KEY AUTOINCREMENT,"
    "name TEXT NOT NULL,"
    "long_name TEXT,"
    "type INTEGER NOT NULL,"
    "is_supported BOOLEAN NOT NULL DEFAULT 0,"
    "creation_date UNSIGNED INT NOT NULL,"
    "UNIQUE(name, type) ON CONFLICT FAIL"
    ")";

  bool result;

  try {
    result = sqlite::Tools::ExecuteRequest(dbConn, req);
  } catch(std::exception& ex) {
    LOG_ERROR(MTEXT("Failed to create table for "), CodecTable::Name, MTEXT(": "), ex.what());
    result = false;
  }

  return result;
}

std::vector<mxp::CodecPtr> mxp::Codec::ListAll(MediaExplorerPtr ml, SortingCriteria sort, bool desc) {
  auto req = "SELECT * FROM " + CodecTable::Name + " ORDER BY name";
  if(desc == true) {
    req += " DESC";
  }

  return FetchAll<ICodec>(ml, req);
}
