/*****************************************************************************
 * Media Library
 *****************************************************************************
 * Copyright (C) 2015 Hugo Beauzée-Luyssen, Videolabs
 *
 * Authors: Hugo Beauzée-Luyssen<hugo@beauzee.fr>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301, USA.
 *****************************************************************************/

#ifndef FS_UNIX_DEVICELISTER_H
#define FS_UNIX_DEVICELISTER_H

#if HAVE_CONFIG_H
# include "config.h"
#endif

#if defined(HAVE_LINUX)

#include "mediaexplorer/IDeviceLister.h"

namespace mxp {
namespace fs {

class DeviceLister : public IDeviceLister {
private:
  // Device name / UUID map
  using DeviceMap = std::unordered_map<std::string, std::string>;
  // Device path / Mountpoints map
  using MountpointMap = std::unordered_map<std::string, std::string>;

  DeviceMap ListDevices() const;
  MountpointMap ListMountpoints() const;
  std::string DeviceFromDeviceMapper(const std::string& devicePath) const;
  bool IsRemovable(const std::string& deviceName, const std::string& mountpoint) const;

public:
  virtual std::vector<std::tuple<std::string, std::string, bool>> Devices() const override;
};

} /* namespace fs */
} /* namespace mxp */

#endif /* HAVE_LINUX */

#endif /* FS_UNIX_DEVICELISTER_H */
