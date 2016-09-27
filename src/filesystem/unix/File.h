/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#ifndef FS_FILE_H
#define FS_FILE_H

#include "filesystem/common/CommonFile.h"

struct stat;

namespace mxp {
namespace fs {

class File : public CommonFile {
public:
  explicit File(const std::string& filePath);

  virtual time_t lastModificationDate() const override;

private:
  mutable time_t m_lastModificationDate;
};

} /* namespace fs */
} /* namespace mxp */

#endif /* FS_FILE_H */
