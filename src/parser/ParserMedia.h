/*****************************************************************************
* Media Explorer
*****************************************************************************/

#ifndef PERSERMEDIA_H
#define PERSERMEDIA_H

#include <memory>
#include "filesystem/IFile.h"
#include "mediaexplorer/Types.h"
#include <Types.h>

namespace mxp {
class Media;
namespace parser {

class Context;

class ParserMedia {

public:
  ParserMedia(MediaExplorerPtr ml, std::shared_ptr<Media> media, const std::shared_ptr<mxp::fs::IFile> file);
  ~ParserMedia();

  ContainerPtr GetMediaContainer();

private:
  const std::shared_ptr<mxp::fs::IFile> m_file;
  std::unique_ptr<Context>              m_context;
};

} /* namespace parser */
} /* namespace mxp */

#endif /* PERSERMEDIA_H */
