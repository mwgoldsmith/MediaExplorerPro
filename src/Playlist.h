/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#pragma once

#include "mediaexplorer/IPlaylist.h"

#include "database/SqliteTools.h"
#include "database/DatabaseHelpers.h"
#include "utils/Cache.h"

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

  static std::shared_ptr<Playlist> create(MediaExplorerPtr ml, const std::string& name);

  virtual int64_t id() const override;
  virtual const std::string& name() const override;
  virtual bool setName(const std::string& name) override;
  virtual time_t creationDate() const override;
  virtual std::vector<MediaPtr> media() const override;
  virtual bool append(int64_t mediaId) override;
  virtual bool add(int64_t mediaId, unsigned int position) override;
  virtual bool move(int64_t mediaId, unsigned int position) override;
  virtual bool remove(int64_t mediaId) override;

  static bool CreateTable(DBConnection dbConn);
  static bool createTriggers(DBConnection dbConn);
  static std::vector<PlaylistPtr> search(MediaExplorerPtr ml, const std::string& name);
  static std::vector<PlaylistPtr> listAll(MediaExplorerPtr ml, SortingCriteria sort, bool desc);

private:
  MediaExplorerPtr m_ml;
  int64_t         m_id;
  std::string     m_name;
  time_t          m_creationDate;

  friend policy::PlaylistTable;
};

}
