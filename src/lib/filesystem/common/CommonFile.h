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
  virtual const std::string& GetName() const override;
  virtual const std::string& GetPath() const override;
  virtual const std::string& GetFullPath() const override;
  virtual const std::string& GetExtension() const override;

protected:
  const std::string m_path;
  const std::string m_name;
  const std::string m_fullPath;
  const std::string m_extension;
};

} /* namespace fs */
} /* namespace mxp */

#endif /* FS_COMMONFILE_H */
