#pragma once

#include <memory>
#include <string>
#include "mediaexplorer/Types.h"
#include "Types.h"

namespace mxp {
class Media;
namespace parser {
class ParserMedia;
}
namespace factory {

class ParserMediaFactory {
public:
  explicit ParserMediaFactory(MediaExplorerPtr ml);

  std::shared_ptr<parser::ParserMedia> CreateParserMedia(std::shared_ptr<Media> media, const std::shared_ptr<mxp::fs::IFile> file);
  std::shared_ptr<parser::ParserMedia> CreateParserMedia(std::shared_ptr<Media> media, const std::string& file);

private:
  MediaExplorerPtr m_ml;
};

}
}
