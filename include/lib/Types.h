/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#ifndef TYPES_H
#define TYPES_H

#include <memory>

namespace mxp {

class Album;
class AlbumTrack;
class Artist;
class DiscovererWorker;
class Genre;
class Image;
class Media;
class MediaContext;
class MediaDevice;
class MediaExplorer;
class MediaFile;
class MediaFolder;
class ModificationNotifier;
class Movie;
class Parser;
class ParserService;
class Show;
class ShowEpisode;
class SqliteConnection;
class Stream;
class VideoTrack;
enum class SettingsKey;

namespace factory {
class IFileSystem;
}

namespace fs {
class IFile;
class IDirectory;
}

using MediaContextPtr  = std::shared_ptr<MediaContext>;
using ImagePtr         = std::shared_ptr<Image>;

typedef SqliteConnection* DBConnection;
using MediaExplorerPtr = const MediaExplorer*;

} 

#endif /* TYPES_H */

