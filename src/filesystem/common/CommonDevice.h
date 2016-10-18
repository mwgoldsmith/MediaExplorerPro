/*****************************************************************************
* Media Explorer
*****************************************************************************/

#pragma once

#include "filesystem/IDevice.h"

namespace mxp {
namespace fs {

class CommonDevice : public IDevice {
public:
  CommonDevice(const std::string& uuid, const std::string& mountpoint, bool isRemovable);
  virtual const std::string& uuid() const override;
  virtual bool IsRemovable() const override;
  virtual bool isPresent() const override;
  virtual const std::string& mountpoint() const override;

private:
  std::string m_uuid;
  std::string m_mountpoint;
  bool m_present;
  bool m_removable;
};

}
}
