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
  static const mstring Name;
  static const mstring PrimaryKeyColumn;
  static int64_t Playlist::*const PrimaryKey;
};
}

class Playlist : public IPlaylist, public DatabaseHelpers<Playlist, policy::PlaylistTable> {
public:
  Playlist(MediaExplorerPtr ml, sqlite::Row& row);

  Playlist(MediaExplorerPtr ml, const mstring& name);

  virtual int64_t Id() const override;

  virtual const mstring& GetName() const override;

  virtual bool SetName(const mstring& name) override;

  virtual time_t GetCreationDate() const override;

  virtual std::vector<MediaPtr> GetMedia() const override;

  virtual bool Append(int64_t mediaId) override;

  virtual bool Add(int64_t mediaId, unsigned int position) override;

  virtual bool Move(int64_t mediaId, unsigned int position) override;

  virtual bool Remove(int64_t mediaId) override;


  static bool CreateTable(DBConnection connection);

  static bool CreateTriggers(DBConnection connection);

  static std::shared_ptr<Playlist> Create(MediaExplorerPtr ml, const mstring& name) noexcept;

  static std::vector<PlaylistPtr> Search(MediaExplorerPtr ml, const mstring& name);

  static std::vector<PlaylistPtr> ListAll(MediaExplorerPtr ml, SortingCriteria sort, bool desc);

private:
  MediaExplorerPtr m_ml;
  int64_t          m_id;
  mstring          m_name;
  time_t           m_creationDate;

  friend policy::PlaylistTable;
};

} /* namespace mxp */

#endif /* PLAYLIST_H */
