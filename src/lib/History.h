﻿/*****************************************************************************
* Media Explorer
*****************************************************************************/

#ifndef HISTORY_H
#define HISTORY_H

#include "database/DatabaseHelpers.h"
#include "mediaexplorer/IHistoryEntry.h"
#include "mediaexplorer/Common.h"
#include "Types.h"

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

  virtual const std::string& mrl() const override;

  virtual unsigned int InsertionDate() const override;

  virtual bool IsFavorite() const override;

  virtual bool SetFavorite(bool isFavorite) override;

  static bool CreateTable(DBConnection dbConnection);

  static bool insert(DBConnection dbConn, const std::string& mrl);

  static std::vector<HistoryPtr> fetch(MediaExplorerPtr ml);

  static bool clearStreams(MediaExplorerPtr ml);

  static constexpr unsigned int MaxEntries = 100u;

private:
  MediaExplorerPtr m_ml;
  int64_t          m_id;
  std::string      m_mrl;
  unsigned int     m_date;
  bool             m_favorite;

  friend policy::HistoryTable;
};

} /* namespace mxp */

#endif /* HISTORY_H */
