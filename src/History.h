/*****************************************************************************
* Media Explorer
*****************************************************************************/

#ifndef HISTORY_H
#define HISTORY_H

#include <string>
#include <vector>
#include "Types.h"
#include "database/DatabaseHelpers.h"
#include "mediaexplorer/IHistoryEntry.h"

namespace mxp {

class History;
class Media;

namespace policy {
struct HistoryTable {
  static const std::string Name;
  static const std::string PrimaryKeyColumn;
  static int64_t History::* const PrimaryKey;
};
}

class History : public IHistoryEntry, public DatabaseHelpers<History, policy::HistoryTable> {
public:
  History(MediaExplorerPtr ml, sqlite::Row& row);
  static bool CreateTable(DBConnection dbConnection);
  static bool insert(DBConnection dbConn, const std::string& mrl);
  static std::vector<HistoryPtr> fetch(MediaExplorerPtr ml);
  static bool clearStreams(MediaExplorerPtr ml);

  virtual const std::string& mrl() const override;
  virtual unsigned int insertionDate() const override;
  virtual bool isFavorite() const override;
  virtual bool setFavorite(bool isFavorite) override;

  static constexpr unsigned int MaxEntries = 100u;

private:
  MediaExplorerPtr m_ml;

  int64_t m_id;
  std::string m_mrl;
  unsigned int m_date;
  bool m_favorite;

  friend policy::HistoryTable;
};

} /* namespace mxp */

#endif /* HISTORY_H */
