/*****************************************************************************
* Media Explorer
*****************************************************************************/

#include "stdafx.h"
#if HAVE_CONFIG_H
#  include "config.h"
#endif

#include "CommonDevice.h"
#include "utils/Filename.h"

namespace mxp {
namespace fs {

CommonDevice::CommonDevice(const std::string& uuid, const std::string& mountpoint, bool isRemovable)
  : m_uuid(uuid)
  , m_mountpoint(utils::file::toFolderPath(mountpoint))
  , m_present(true)
  , m_removable(isRemovable) {
}

const std::string& CommonDevice::uuid() const {
  return m_uuid;
}

bool CommonDevice::IsRemovable() const {
  return m_removable;
}

bool CommonDevice::isPresent() const {
  return m_present;
}

const std::string& CommonDevice::mountpoint() const {
  return m_mountpoint;
}


}
}
