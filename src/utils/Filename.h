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

#include <string>

namespace mxp {

namespace utils {

namespace file {
  std::string extension( const std::string& fileName );
  std::string directory( const std::string& filePath );
  std::string fileName( const std::string& filePath );
  std::string firstFolder( const std::string& path );
  std::string removePath( const std::string& fullPath, const std::string& toRemove );
  std::string parentDirectory( const std::string& path );
  /**
   * @brief toFolder  Ensures a path is a folder path; ie. it has a terminal '/'
   * @param path    The path to sanitize
   */
  std::string& toFolderPath( std::string& path );
  std::string  toFolderPath( const std::string& path );
  std::string  toAbsolutePath( const std::string& path );
}

}

}

