#pragma once

#include <memory>
#include <string>
#include "parser/ParserMedia.h"
#include "factory/IFileSystem.h"
#include "mediaexplorer/Types.h"
#include "Types.h"

namespace mxp {
namespace factory {

class ParserMediaFactory {
public:
  explicit ParserMediaFactory(MediaExplorerPtr ml);

  std::shared_ptr<parser::ParserMedia> CreateParserMedia(const std::shared_ptr<mxp::fs::IFile> file);
  std::shared_ptr<parser::ParserMedia> CreateParserMedia(const std::string& file);

private:
  MediaExplorerPtr m_ml;
};

}
}
