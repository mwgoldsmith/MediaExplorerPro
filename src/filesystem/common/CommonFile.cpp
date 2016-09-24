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

#if HAVE_CONFIG_H
# include "config.h"
#endif

#include "CommonFile.h"
#include "utils/Filename.h"

namespace mxp {

namespace fs {

CommonFile::CommonFile(const std::string& filePath)
  : m_path(utils::file::directory(filePath))
  , m_name(utils::file::fileName(filePath))
  , m_fullPath(filePath)
  , m_extension(utils::file::extension(filePath)) {
}

const std::string& CommonFile::name() const {
  return m_name;
}

const std::string& CommonFile::path() const {
  return m_path;
}

const std::string& CommonFile::fullPath() const {
  return m_fullPath;
}

const std::string& CommonFile::extension() const {
  return m_extension;
}

}

}
