/*****************************************************************************
* Media Explorer
*****************************************************************************/

#ifndef PARSERMEDIA_H
#define PARSERMEDIA_H

#include "filesystem/IFile.h"
#include "mediaexplorer/Common.h"
#include "Types.h"

namespace mxp {
class Media;

namespace parser {
struct Task;

class ParserMedia {
public:
  ParserMedia(MediaExplorerPtr ml, parser::Task &task);
  ~ParserMedia() = default;
private:
  const MediaExplorerPtr                m_ml;
};

} /* namespace parser */
} /* namespace mxp */

#endif /* PARSERMEDIA_H */
