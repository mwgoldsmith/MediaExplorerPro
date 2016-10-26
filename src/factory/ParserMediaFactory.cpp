/*****************************************************************************
* Media Explorer
*****************************************************************************/

#include "stdafx.h"
#if HAVE_CONFIG_H
#  include "config.h"
#endif

#include "av/MediaController.h"
#include "factory/ParserMediaFactory.h"
#include "parser/ParserMedia.h"
#include "logging/Logger.h"
#include "MediaExplorer.h"
#include "mediaexplorer/Common.h"

mxp::factory::ParserMediaFactory::ParserMediaFactory(MediaExplorerPtr ml)
  : m_ml(ml) {
}

std::shared_ptr<mxp::parser::ParserMedia> mxp::factory::ParserMediaFactory::CreateParserMedia(std::shared_ptr<Media> media, const std::shared_ptr<mxp::fs::IFile> file) {
  LOG_INFO("Creating ParserMedia for ", file->GetName());
  return  nullptr;// std::make_shared<parser::ParserMedia>(m_ml, {});
}

std::shared_ptr<mxp::parser::ParserMedia> mxp::factory::ParserMediaFactory::CreateParserMedia(std::shared_ptr<Media> media, const std::string & file) {
  auto ifile = m_ml->GetFileSystem()->CreateFileFromPath(file);
  LOG_INFO("Creating ParserMedia for ", ifile->GetName());
  return  nullptr;// std::make_shared<parser::ParserMedia>(m_ml, media, ifile);
}
