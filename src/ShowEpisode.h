/*****************************************************************************
* Media Explorer
*****************************************************************************/

#ifndef SHOWEPISODE_H
#define SHOWEPISODE_H

#include "database/DatabaseHelpers.h"
#include "mediaexplorer/IShowEpisode.h"
#include "mediaexplorer/Common.h"
#include "Types.h"

namespace mxp {

namespace policy {
struct ShowEpisodeTable {
  static const std::string Name;
  static const std::string PrimaryKeyColumn;
  static int64_t ShowEpisode::*const PrimaryKey;
};
}

class ShowEpisode : public IShowEpisode, public DatabaseHelpers<ShowEpisode, policy::ShowEpisodeTable> {
public:
  ShowEpisode(MediaExplorerPtr ml, sqlite::Row& row);

  ShowEpisode(MediaExplorerPtr ml, int64_t mediaId, const std::string& name, unsigned int episodeNumber, int64_t showId);

  virtual int64_t Id() const override;

  virtual const std::string& GetArtworkMrl() const override;

  virtual unsigned int episodeNumber() const override;

  virtual const std::string& GetName() const override;

  virtual const std::string& GetShortSummary() const override;

  virtual const std::string& GetTvdbId() const override;

  virtual ShowPtr show() override;

  virtual std::vector<MediaPtr> files() override;

  bool SetArtworkMrl(const std::string& artworkMrl);

  unsigned int seasonNumber() const override;

  bool setSeasonNumber(unsigned int seasonNumber);

  bool SetShortSummary(const std::string& summary);

  bool SetTvdbId(const std::string& tvdbId);

  static bool CreateTable(DBConnection dbConnection);

  static std::shared_ptr<ShowEpisode> Create(MediaExplorerPtr ml, int64_t mediaId, const std::string& title, unsigned int episodeNumber, int64_t showId);

  static ShowEpisodePtr FindByMedia(MediaExplorerPtr ml, int64_t mediaId);

private:
  MediaExplorerPtr m_ml;
  int64_t          m_id;
  int64_t          m_mediaId;
  std::string      m_artworkMrl;
  unsigned int     m_episodeNumber;
  std::string      m_name;
  unsigned int     m_seasonNumber;
  std::string      m_shortSummary;
  std::string      m_tvdbId;
  int64_t          m_showId;
  ShowPtr          m_show;

  friend struct policy::ShowEpisodeTable;
};

} /* namespace mxp */

#endif /* SHOWEPISODE_H */
