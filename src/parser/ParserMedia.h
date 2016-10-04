/*****************************************************************************
* Media Explorer
*****************************************************************************/

#ifndef PERSERMEDIA_H
#define PERSERMEDIA_H

#include <memory>
#include "filesystem/IFile.h"
#include "parser/MediaContainer.h"

namespace mxp {
namespace parser {

class Context;

class ParserMedia {

public:
  explicit ParserMedia(const std::shared_ptr<mxp::fs::IFile> file);
  ~ParserMedia();

  std::shared_ptr<MediaContainer> GetMediaContainer();

private:
  const std::shared_ptr<mxp::fs::IFile> m_file;
  std::unique_ptr<Context>              m_context;
};

} /* namespace parser */
} /* namespace mxp */

#endif /* PERSERMEDIA_H */
