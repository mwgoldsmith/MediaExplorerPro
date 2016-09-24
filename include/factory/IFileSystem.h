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

#pragma once

#include <memory>
#include <string>

namespace mxp {

namespace fs {
  class IDirectory;
  class IFile;
  class IDevice;
}

namespace factory {
class IFileSystem {
public:
  virtual ~IFileSystem() = default;

  virtual std::shared_ptr<fs::IFile> CreateFile( const std::string& path ) = 0;
  ///
  /// \brief CreateDirectory creates a representation of a directory
  /// \param path An absolute path to a directory
  ///
  virtual std::shared_ptr<fs::IDirectory> CreateDirectory( const std::string& path ) = 0;
  ///
  /// \brief CreateDevice creates a representation of a device
  /// \param uuid The device UUID
  /// \return A representation of the device, or nullptr if the device is currently unavailable.
  ///
  virtual std::shared_ptr<fs::IDevice> CreateDevice( const std::string& uuid ) = 0;
  ///
  /// \brief CreateDeviceFromPath creates a representation of a device
  /// \param path A path.
  /// \return A representation of the device.
  ///
  virtual std::shared_ptr<fs::IDevice> CreateDeviceFromPath( const std::string& path ) = 0;
  ///
  /// \brief refresh Will cause any FS cache to be refreshed.
  ///
  virtual void Refresh() = 0;
};
}

}
