/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#ifndef LABEL_H
#define LABEL_H

#include <string>
#include "mediaexplorer/ILabel.h"
#include "database/DatabaseHelpers.h"

namespace mxp {

class Media;
class Label;

namespace policy {
struct LabelTable {
  static const std::string Name;
  static const std::string PrimaryKeyColumn;
  static int64_t Label::*const PrimaryKey;
};
}

class Label : public ILabel, public DatabaseHelpers<Label, policy::LabelTable> {
public:
  Label(MediaExplorerPtr ml, sqlite::Row& row);
  Label(MediaExplorerPtr ml, const std::string& name);

public:
  virtual int64_t id() const override;
  virtual const std::string& name() const override;
  virtual std::vector<MediaPtr> Files() override;

  static bool CreateTable(DBConnection dbConnection);
  static LabelPtr Create(MediaExplorerPtr ml, const std::string& name);

private:
  MediaExplorerPtr m_ml;
  int64_t          m_id;
  std::string      m_name;

  friend struct policy::LabelTable;
};

} /* namespace mxp */

#endif /* LABEL_H */
