/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#ifndef PLAYLIST_H
#define PLAYLIST_H

#include "database/DatabaseHelpers.h"
#include "mediaexplorer/IPlaylist.h"
#include "mediaexplorer/Common.h"
#include "Types.h"

namespace mxp {

class Playlist;

namespace policy {
struct PlaylistTable {
  static const std::string Name;
  static const std::string PrimaryKeyColumn;
  static int64_t Playlist::*const PrimaryKey;
};
}

class Playlist : public IPlaylist, public DatabaseHelpers<Playlist, policy::PlaylistTable> {
public:
  Playlist(MediaExplorerPtr ml, sqlite::Row& row);

  Playlist(MediaExplorerPtr ml, const std::string& name);

  virtual int64_t Id() const override;

  virtual const std::string& Name() const override;

  virtual bool SetName(const std::string& name) override;

  virtual time_t CreationDate() const override;

  virtual std::vector<MediaPtr> media() const override;

  virtual bool append(int64_t mediaId) override;

  virtual bool add(int64_t mediaId, unsigned int position) override;

  virtual bool move(int64_t mediaId, unsigned int position) override;

  virtual bool remove(int64_t mediaId) override;


  static bool CreateTable(DBConnection dbConn);

  static std::shared_ptr<Playlist> Create(MediaExplorerPtr ml, const std::string& name);

  static bool CreateTriggers(DBConnection dbConn);

  static std::vector<PlaylistPtr> Search(MediaExplorerPtr ml, const std::string& name);

  static std::vector<PlaylistPtr> ListAll(MediaExplorerPtr ml, SortingCriteria sort, bool desc);

private:
  MediaExplorerPtr m_ml;
  int64_t         m_id;
  std::string     m_name;
  time_t          m_creationDate;

  friend policy::PlaylistTable;
};

} /* namespace mxp */

#endif /* PLAYLIST_H */
