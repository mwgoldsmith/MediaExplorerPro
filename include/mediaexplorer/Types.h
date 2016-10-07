/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#ifndef MXP_TYPES_H
#define MXP_TYPES_H

#include <memory>

namespace mxp {

class IAudioTrack;
class ICodec;
class IContainer;
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
class IMetadata;
class IPlaylist;
class IStream;
class IVideoTrack;

namespace factory {

class IFileSystem;

} /* namespace factory */

namespace fs {

class IDevice;
class IDirectory;
class IFile;

} /* namespace fs */

using AudioTrackPtr   = std::shared_ptr<IAudioTrack>;
using CodecPtr        = std::shared_ptr<ICodec>;
using ContainerPtr    = std::shared_ptr<IContainer>;
using DeviceListerPtr = std::shared_ptr<IDeviceLister>;
using FileSystemPtr   = std::shared_ptr<factory::IFileSystem>;
using DevicePtr       = std::shared_ptr<fs::IDevice>;
using DirectoryPtr    = std::shared_ptr<fs::IDirectory>;
using FilePtr         = std::shared_ptr<fs::IFile>;
using GenrePtr        = std::shared_ptr<IGenre>;
using HistoryPtr      = std::shared_ptr<IHistoryEntry>;
using LabelPtr        = std::shared_ptr<ILabel>;
using MediaPtr        = std::shared_ptr<IMedia>;
using MediaDevicePtr  = std::shared_ptr<IMediaDevice>;
using MediaFolderPtr  = std::shared_ptr<IMediaFolder>;
using MediaFilePtr    = std::shared_ptr<IMediaFile>;
using MetadataPtr     = std::shared_ptr<IMetadata>;
using PlaylistPtr     = std::shared_ptr<IPlaylist>;
using StreamPtr       = std::shared_ptr<IStream>;
using VideoTrackPtr   = std::shared_ptr<IVideoTrack>;

} /* namespace mxp */

#endif /* MXP_TYPES_H */

