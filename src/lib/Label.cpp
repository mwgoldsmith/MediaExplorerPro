/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#include "stdafx.h"
#if HAVE_CONFIG_H
# include "config.h"
#endif

#include "Label.h"
#include "Media.h"
#include "database/SqliteTools.h"

using mxp::policy::LabelTable;
const std::string LabelTable::Name = "Label";
const std::string LabelTable::PrimaryKeyColumn = "id_label";
int64_t mxp::Label::* const LabelTable::PrimaryKey = &mxp::Label::m_id;

mxp::Label::Label(MediaExplorerPtr ml, sqlite::Row& row)
  : m_ml(ml) {
  row >> m_id
    >> m_name;
}

mxp::Label::Label(MediaExplorerPtr ml, const std::string& name)
  : m_ml(ml)
  , m_id(0)
  , m_name(name) {
}

int64_t mxp::Label::Id() const {
  return m_id;
}

const std::string& mxp::Label::Name() const {
  return m_name;
}

std::vector<mxp::MediaPtr> mxp::Label::Files() {
  static const auto req = "SELECT f.* FROM " + policy::MediaTable::Name + " f "
    "INNER JOIN LabelFileRelation lfr ON lfr.media_id = f." + policy::MediaTable::PrimaryKeyColumn + " " +
    "WHERE lfr.label_id = ?";
  return Media::FetchAll<IMedia>(m_ml, req, m_id);
}

mxp::LabelPtr mxp::Label::Create(MediaExplorerPtr ml, const std::string& name) {
  auto self = std::make_shared<Label>(ml, name);
  auto req = "INSERT INTO Label VALUES(NULL, ?)";
  if (insert(ml, self, req, self->m_name) == false)
    return nullptr;
  return self;
}

bool mxp::Label::CreateTable(DBConnection dbConnection) {
  static const auto req = "CREATE TABLE IF NOT EXISTS " + LabelTable::Name + "(" + 
    LabelTable::PrimaryKeyColumn + " INTEGER PRIMARY KEY AUTOINCREMENT, "
    "name TEXT UNIQUE ON CONFLICT FAIL"
    ")";
  static const auto relReq = "CREATE TABLE IF NOT EXISTS LabelFileRelation("
    "label_id INTEGER,"
    "media_id INTEGER,"
    "PRIMARY KEY(label_id, media_id),"
    "FOREIGN KEY(label_id) REFERENCES " + LabelTable::Name + "(" + LabelTable::PrimaryKeyColumn + ") ON DELETE CASCADE,"
    "FOREIGN KEY(media_id) REFERENCES " + policy::MediaTable::Name + "(" + policy::MediaTable::PrimaryKeyColumn + ") ON DELETE CASCADE);";
  static const auto ftsTrigger = "CREATE TRIGGER IF NOT EXISTS delete_label_fts "
    "BEFORE DELETE ON " + LabelTable::Name +
    " BEGIN"
    " UPDATE " + policy::MediaTable::Name + "Fts SET labels = TRIM(REPLACE(labels, old.name, ''))"
    " WHERE labels MATCH old.name;"
    " END";

  return sqlite::Tools::ExecuteRequest(dbConnection, req) &&
      sqlite::Tools::ExecuteRequest(dbConnection, relReq) &&
      sqlite::Tools::ExecuteRequest(dbConnection, ftsTrigger);
}
