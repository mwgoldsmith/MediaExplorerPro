/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#ifndef FS_COMMONFILE_H
#define FS_COMMONFILE_H

#include "filesystem/IFile.h"

namespace mxp {
namespace fs {

class CommonFile : public IFile {
public:
  explicit CommonFile(const std::string& filePath);
  virtual const std::string& name() const override;
  virtual const std::string& path() const override;
  virtual const std::string& fullPath() const override;
  virtual const std::string& extension() const override;

protected:
  const std::string m_path;
  const std::string m_name;
  const std::string m_fullPath;
  const std::string m_extension;
};

} /* namespace fs */
} /* namespace mxp */

#endif /* FS_COMMONFILE_H */
