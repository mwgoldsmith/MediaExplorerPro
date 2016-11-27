/*****************************************************************************
* Media Explorer
*****************************************************************************/

#include "stdafx.h"
#if HAVE_CONFIG_H
# include "config.h"
#endif

#include "Device.h"

mxp::fs::Device::Device(const std::string& uuid, const std::string& mountpoint, bool isRemovable)
  : CommonDevice(uuid, mountpoint, isRemovable) {
}
