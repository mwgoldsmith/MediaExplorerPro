/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#if HAVE_CONFIG_H
# include "config.h"
#endif

#include <cassert>
#include <cstdlib>
#include <cstring>

#include "Label.h"
#include "Media.h"
#include "database/SqliteTools.h"

namespace mxp {

const std::string policy::LabelTable::Name = "Label";
const std::string policy::LabelTable::PrimaryKeyColumn = "id_label";
int64_t Label::* const policy::LabelTable::PrimaryKey = &Label::m_id;

Label::Label(MediaExplorerPtr ml, sqlite::Row& row)
  : m_ml(ml) {
  row >> m_id
    >> m_name;
}

Label::Label(MediaExplorerPtr ml, const std::string& name)
  : m_ml(ml)
  , m_id(0)
  , m_name(name) {
}

int64_t Label::id() const {
  return m_id;
}

const std::string& Label::name() const {
  return m_name;
}

std::vector<MediaPtr> Label::files() {
  static const std::string req = "SELECT f.* FROM " + policy::MediaTable::Name + " f "
      "INNER JOIN LabelFileRelation lfr ON lfr.media_id = f.id_media "
      "WHERE lfr.label_id = ?";
  return Media::FetchAll<IMedia>(m_ml, req, m_id);
}

LabelPtr Label::create(MediaExplorerPtr ml, const std::string& name) {
  auto self = std::make_shared<Label>(ml, name);
  const char* req = "INSERT INTO Label VALUES(NULL, ?)";
  if (insert(ml, self, req, self->m_name) == false)
    return nullptr;
  return self;
}

bool Label::createTable(DBConnection dbConnection) {
  static const std::string req = "CREATE TABLE IF NOT EXISTS " + policy::LabelTable::Name + "("
      "id_label INTEGER PRIMARY KEY AUTOINCREMENT, "
      "name TEXT UNIQUE ON CONFLICT FAIL"
      ")";
  static const std::string relReq = "CREATE TABLE IF NOT EXISTS LabelFileRelation("
      "label_id INTEGER,"
      "media_id INTEGER,"
      "PRIMARY KEY (label_id, media_id),"
      "FOREIGN KEY(label_id) REFERENCES Label(id_label) ON DELETE CASCADE,"
      "FOREIGN KEY(media_id) REFERENCES Media(id_media) ON DELETE CASCADE);";
  static const std::string ftsTrigger = "CREATE TRIGGER IF NOT EXISTS delete_label_fts "
      "BEFORE DELETE ON " + policy::LabelTable::Name +
      " BEGIN"
      " UPDATE " + policy::MediaTable::Name + "Fts SET labels = TRIM(REPLACE(labels, old.name, ''))"
      " WHERE labels MATCH old.name;"
      " END";

  return sqlite::Tools::executeRequest(dbConnection, req) &&
      sqlite::Tools::executeRequest(dbConnection, relReq) &&
      sqlite::Tools::executeRequest(dbConnection, ftsTrigger);
}

}
