/*****************************************************************************
* Media Explorer
*****************************************************************************/

#ifndef SHOW_H
#define SHOW_H

#include "database/DatabaseHelpers.h"
#include "mediaexplorer/IShow.h"
#include "mediaexplorer/Types.h"
#include "Types.h"

namespace mxp {

class Media;
class Show;
class ShowEpisode;

namespace policy {
struct ShowTable {
  static const std::string Name;
  static const std::string PrimaryKeyColumn;
  static int64_t Show::*const PrimaryKey;
};
}

class Show : public IShow, public DatabaseHelpers<Show, policy::ShowTable> {
public:
  Show(MediaExplorerPtr ml, sqlite::Row& row);
  Show(MediaExplorerPtr ml, const std::string& name);

  virtual int64_t Id() const override;
  virtual const std::string& name() const override;
  virtual time_t releaseDate() const override;
  bool setReleaseDate(time_t date);
  virtual const std::string& shortSummary() const override;
  bool setShortSummary(const std::string& summary);
  virtual const std::string& artworkMrl() const override;
  bool setArtworkMrl(const std::string& artworkMrl);
  virtual const std::string& tvdbId() override;
  bool setTvdbId(const std::string& summary);
  std::shared_ptr<ShowEpisode> addEpisode(Media& media, const std::string& title, unsigned int episodeNumber);
  virtual std::vector<ShowEpisodePtr> episodes() override;

  static bool createTable(DBConnection dbConnection);
  static std::shared_ptr<Show> create(MediaExplorerPtr ml, const std::string& name);

protected:
  MediaExplorerPtr m_ml;

  int64_t m_id;
  std::string m_name;
  time_t m_releaseDate;
  std::string m_shortSummary;
  std::string m_artworkMrl;
  std::string m_tvdbId;

  friend struct policy::ShowTable;
};

}

#endif // SHOW_H
