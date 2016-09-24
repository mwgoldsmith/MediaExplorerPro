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
    * @brief onFileAdded Will be called when some media get added.
    * Depending if the media is being restored or was just discovered,
    * the media type might be a best effort guess. If the media was freshly
    * discovered, it is extremely likely that no metadata will be
    * available yet.
    * The number of media is undefined, but is guaranteed to be at least 1.
    */
  virtual void onMediaAdded(std::vector<MediaPtr> media) = 0;
  /**
  * @brief onFileUpdated Will be called when a file metadata gets updated.
  */
  virtual void onMediaUpdated(std::vector<MediaPtr> media) = 0;

  virtual void onMediaDeleted(std::vector<int64_t> ids) = 0;
  /**
  * @brief onParsingStatsUpdated Called when the parser statistics are updated
  *
  * There is no waranty about how often this will be called.
  * @param percent The progress percentage [0,100]
  *
  */
  virtual void onParsingStatsUpdated(uint32_t percent) = 0;
  /**
  * @brief onDiscoveryStarted This callback will be invoked when a folder queued for discovery
  * (by calling IMediaLibrary::Discover()) gets processed.
  * @param entryPoint The entrypoint being discovered
  * This callback will be invoked once per endpoint.
  * It will also be invoked when the initial medialibrary reload starts, in which case, the
  * entryPoint will be an empty string.
  */
  virtual void onDiscoveryStarted(const std::string& entryPoint) = 0;
  /**
  * @brief onDiscoveryProgress This callback will be invoked each time the discoverer enters a new
  * entrypoint. Typically, everytime it enters a new folder.
  * @param entryPoint The entrypoint being discovered
  * This callback can be invoked multiple times even though a single entry point was asked to be
  * discovered. ie. In the case of a file system discovery, discovering a folder would make this
  * callback being invoked for all subfolders
  */
  virtual void onDiscoveryProgress(const std::string& entryPoint) = 0;
  /**
  * @brief onDiscoveryCompleted Will be invoked when the discovery of a specified entrypoint has
  * completed.
  * ie. in the case of a filesystem discovery, once the folder, and all its files and subfolders
  * have been discovered.
  * This will also be invoked with an empty entryPoint when the initial reload of the medialibrary
  * has completed.
  */
  virtual void onDiscoveryCompleted(const std::string& entryPoint) = 0;
};

} /* namespace mxp */

#endif /* MXP_IMEDIAEXPLORERCB_H */
