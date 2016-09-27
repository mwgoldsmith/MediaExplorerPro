/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#ifndef MXP_TYPES_H
#define MXP_TYPES_H

#include <memory>

namespace mxp {

class IAudioTrack;
class IDeviceLister;
class IDeviceListerCb;
class IGenre;
class IHistoryEntry;
class ILabel;
class ILogger;
class IMedia;
class IMediaExplorer;
class IMediaExplorerCb;
class IMediaDevice;
class IMediaFile;
class IMediaFolder;
class IPlaylist;
class IVideoTrack;

namespace factory { class IFileSystem; }

using AudioTrackPtr   = std::shared_ptr<IAudioTrack>;
using DeviceListerPtr = std::shared_ptr<IDeviceLister>;
using FileSystemPtr   = std::shared_ptr<factory::IFileSystem>;
using GenrePtr        = std::shared_ptr<IGenre>;
using HistoryPtr      = std::shared_ptr<IHistoryEntry>;
using LabelPtr        = std::shared_ptr<ILabel>;
using MediaPtr        = std::shared_ptr<IMedia>;
using MediaFilePtr    = std::shared_ptr<IMediaFile>;
using PlaylistPtr     = std::shared_ptr<IPlaylist>;
using VideoTrackPtr   = std::shared_ptr<IVideoTrack>;
}

#endif /* MXP_TYPES_H */

