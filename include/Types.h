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
class Show;
class ShowEpisode;
class Stream;
class VideoTrack;

class SqliteConnection;
class MediaExplorer;

typedef SqliteConnection* DBConnection;
using MediaExplorerPtr = const MediaExplorer*;

}

#endif /* TYPES_H */

