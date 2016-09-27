/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#ifndef MXP_IMEDIAEXPLORERCB_H
#define MXP_IMEDIAEXPLORERCB_H

#include <vector>
#include <string>

namespace mxp {

class IMediaExplorerCb {
public:
  virtual ~IMediaExplorerCb() = default;
  
  /**
   * @brief onMediaAdded will be called when some media get added.
   *
   * Depending if the media is being restored or was just discovered, the media type might be a
   * best effort guess. If the media was freshly discovered, it is extremely likely that no metadata
   * will be available yet. The number of media is undefined, but is guaranteed to be at least 1.
   */
  virtual void onMediaAdded(std::vector<MediaPtr> media) = 0;

  /**
   * @brief onMediaUpdated will be called when a file metadata gets updated.
   */
  virtual void onMediaUpdated(std::vector<MediaPtr> media) = 0;

  virtual void onMediaDeleted(std::vector<int64_t> ids) = 0;
  /**
   * @brief onParsingStatsUpdated Called when the parser statistics are updated
   *
   * @param percent The progress percentage [0,100]
   *
   * There is no warranty about how often this will be called.
   */
  virtual void onParsingStatsUpdated(uint32_t percent) = 0;

  /**
   * @brief onDiscoveryStarted will be invoked when a folder queued for discovery (by calling 
   *        IMediaExplorer::Discover()) gets processed.
   *
   * @param entryPoint The entry point being discovered
   *
   * This callback will be invoked once per endpoint. It will also be invoked when the initial
   * mediaexplorer reload starts, in which case, the
   * entryPoint will be an empty string.
   */
  virtual void onDiscoveryStarted(const std::string& entryPoint) = 0;

  /**
   * @brief onDiscoveryProgress will be invoked each time the discoverer enters a new entry point.
   *        Typically, every time it enters a new folder.
   *
   * @param entryPoint The entry point being discovered
   *
   * This callback can be invoked multiple times even though a single entry point was asked to be
   * discovered. i.e. In the case of a file system discovery, discovering a folder would make this
   * callback being invoked for all sub folders
   */
  virtual void onDiscoveryProgress(const std::string& entryPoint) = 0;

  /**
   * @brief onDiscoveryCompleted will be invoked when the discovery of a specified entry point has
   *        completed. i.e. in the case of a filesystem discovery, once the folder, and all its files
   *        and sub folders have been discovered. This will also be invoked with an empty entryPoint 
   *        when the initial reload of the mediaexplorer has completed.
   */
  virtual void onDiscoveryCompleted(const std::string& entryPoint) = 0;
};

} /* namespace mxp */

#endif /* MXP_IMEDIAEXPLORERCB_H */
