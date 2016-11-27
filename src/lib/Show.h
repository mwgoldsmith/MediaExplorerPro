/*****************************************************************************
* Media Explorer
*****************************************************************************/

#ifndef SHOW_H
#define SHOW_H

#include "database/DatabaseHelpers.h"
#include "mediaexplorer/IShow.h"
#include "mediaexplorer/Common.h"
#include "Types.h"

namespace mxp {

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

  virtual const std::string& GetName() const override;

  virtual time_t GetReleaseDate() const override;

  virtual const std::string& GetShortSummary() const override;

  virtual const std::string& GetArtworkMrl() const override;

  virtual const std::string& GetTvdbId() override;

  virtual std::vector<ShowEpisodePtr> GetEpisodes() override;

  bool SetArtworkMrl(const std::string& artworkMrl);

  bool SetReleaseDate(time_t date);

  bool SetShortSummary(const std::string& summary);

  bool SetTvdbId(const std::string& summary);

  std::shared_ptr<ShowEpisode> AddEpisode(Media& media, const std::string& title, unsigned int episodeNumber);

  static bool CreateTable(DBConnection dbConnection);

  static std::shared_ptr<Show> Create(MediaExplorerPtr ml, const std::string& name);

protected:
  MediaExplorerPtr m_ml;
  int64_t          m_id;
  std::string      m_name;
  time_t           m_releaseDate;
  std::string      m_shortSummary;
  std::string      m_artworkMrl;
  std::string      m_tvdbId;

  friend struct policy::ShowTable;
};

}

#endif // SHOW_H
