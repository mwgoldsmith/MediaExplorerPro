/*****************************************************************************
* Media Explorer
*****************************************************************************/

#ifndef FS_UNIX_DIRECTORY_H
#define FS_UNIX_DIRECTORY_H

#include "stdafx.h"
#if HAVE_CONFIG_H
# include "config.h"
#endif

#ifdef HAVE_LINUX

#include "filesystem/common/CommonDirectory.h"

namespace mxp {

namespace factory { class IFileSystem; }

namespace fs {

class Directory : public CommonDirectory {
public:
  Directory( const std::string& path, factory::IFileSystem& fsFactory );

private:
  virtual void read() const override;
};

} /* namespace fs */

} /* namespace mxp */

#endif /* HAVE_LINUX */

#endif /* FS_UNIX_DIRECTORY_H */
