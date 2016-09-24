/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#ifndef MXP_IMEDIAEXPLORER_H
#define MXP_IMEDIAEXPLORER_H

#include <string>

#include "Types.h"
#include "factory/IFileSystem.h"
#include "mediaexplorer/ILogger.h"
#include "mediaexplorer/ISettings.h"
#include "mediaexplorer/IMediaExplorerCb.h"
#include "mediaexplorer/MediaSearchAggregate.h"
#include "mediaexplorer/SearchAggregate.h"
#include "mediaexplorer/SortingCriteria.h"

namespace mxp {

class IMediaExplorer {
public:
  virtual ~IMediaExplorer() = default;

  virtual bool Initialize(const std::string& dbPath, IMediaExplorerCb* cb) = 0;
  virtual bool Shutdown() = 0;

  virtual std::string& GetVersion() const = 0;
  virtual void SetLogger(ILogger* logger) = 0;
  virtual void SetCallbacks(IMediaExplorerCb* cb) = 0;
  virtual FileSystemPtr GetFileSystem() const = 0;

  virtual LabelPtr CreateLabel(const std::string& label) = 0;
  virtual bool DeleteLabel(LabelPtr label) = 0;

  //virtual std::vector<GenrePtr> GenreLists(SortingCriteria sort, bool desc) const = 0;
  //virtual GenrePtr Genre(int64_t id) const = 0;

  virtual PlaylistPtr CreatePlaylist(const std::string& name) = 0;
  virtual bool DeletePlaylist(int64_t playlistId) = 0;
  virtual std::vector<PlaylistPtr> PlaylistList(SortingCriteria sort, bool desc) = 0;
  virtual PlaylistPtr Playlist(int64_t id) const = 0;

  virtual MediaSearchAggregate SearchMedia(const std::string& title) const = 0;
  virtual std::vector<PlaylistPtr> SearchPlaylists(const std::string& name) const = 0;
  //virtual std::vector<GenrePtr> SearchGenre(const std::string& genre) const = 0;
  virtual SearchAggregate Search(const std::string& pattern) const = 0;

  virtual bool DeleteMedia(int64_t mediaId) const = 0;
  virtual std::vector<MediaPtr> MediaList(SortingCriteria sort, bool desc) = 0;
  virtual MediaPtr Media(int64_t mediaId) const = 0;

  virtual void Reload() = 0;
  virtual void Reload(const std::string& entryPoint) = 0;
  
  /**
   * @brief discover Launch a discovery on the provided entry point.
   * The actuall discovery will run asynchronously, meaning this method will immediatly return.
   * Depending on which discoverer modules where provided, this might or might not work
   * @param entryPoint What to discover.
   */
  virtual void Discover(const std::string& entryPoint) = 0;

  /**
   * @brief pauseBackgroundOperations Will stop potentially CPU intensive background
   * operations, until resumeBackgroundOperations() is called.
   * If an operation is currently running, it will finish before pausing.
   */
  virtual void PauseBackgroundOperations() = 0;

  /**
   * @brief resumeBackgroundOperations Resumes background tasks, previously
   * interrupted by pauseBackgroundOperations().
   */
  virtual void ResumeBackgroundOperations() = 0;
};

} /* namespace mxp */

extern "C" {
  mxp::IMediaExplorer* mxp_newInstance();
}

#endif /* MXP_IMEDIAEXPLORER_H */
