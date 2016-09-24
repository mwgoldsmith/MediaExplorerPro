/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#ifndef TYPES_H
#define TYPES_H

namespace mxp {

class SqliteConnection;
class MediaExplorer;

typedef SqliteConnection* DBConnection;
using MediaExplorerPtr = const MediaExplorer*;

}

#endif /* TYPES_H */

