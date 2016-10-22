/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#ifndef TYPES_H
#define TYPES_H

namespace mxp {

class Album;
class AlbumTrack;
class Artist;
class Genre;
class Media;
class MediaExplorer;
class Show;
class ShowEpisode;
class SqliteConnection;
class Stream;
class VideoTrack;

typedef SqliteConnection* DBConnection;
using MediaExplorerPtr = const MediaExplorer*;

}

#endif /* TYPES_H */

