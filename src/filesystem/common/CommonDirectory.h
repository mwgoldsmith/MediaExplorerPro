/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#pragma once

#include "filesystem/IDirectory.h"
#include "utils/Cache.h"

namespace mxp {

namespace factory { class IFileSystem; }

namespace fs {

class CommonDirectory : public IDirectory {
public:
  CommonDirectory(const std::string& path, factory::IFileSystem& fsFactory);
  virtual const std::string& path() const override;
  virtual const std::vector<std::shared_ptr<mxp::fs::IFile>>& files() const override;
  virtual const std::vector<std::shared_ptr<mxp::fs::IDirectory>>& dirs() const override;
  virtual std::shared_ptr<IDevice> device() const override;

protected:
  virtual void read() const = 0;

protected:
  std::string m_path;
  mutable std::vector<std::shared_ptr<mxp::fs::IFile>> m_files;
  mutable std::vector<std::shared_ptr<mxp::fs::IDirectory>> m_dirs;
  mutable Cache<std::shared_ptr<IDevice>> m_device;
  factory::IFileSystem& m_fsFactory;
};

}
}
