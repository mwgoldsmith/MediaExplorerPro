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
  Label( MediaExplorerPtr ml, sqlite::Row& row);
  Label( MediaExplorerPtr ml, const std::string& name);

public:
  virtual int64_t id() const override;
  virtual const std::string& name() const override;
  virtual std::vector<MediaPtr> files() override;

  static LabelPtr create( MediaExplorerPtr ml, const std::string& name);
  static bool createTable( DBConnection dbConnection);

private:
  MediaExplorerPtr m_ml;
  int64_t m_id;
  std::string m_name;

  friend struct policy::LabelTable;
};

}

#endif // LABEL_H
