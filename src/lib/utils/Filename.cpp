/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#include "stdafx.h"
#if HAVE_CONFIG_H
# include "config.h"
#endif

#include "utils/Filename.h"

#ifdef HAVE_WIN32
#define DIR_SEPARATOR '\\'
#include "utils/Charsets.h"
#else
#define DIR_SEPARATOR '/'
#endif

std::string mxp::utils::file::extension(const std::string& fileName) {
  auto pos = fileName.find_last_of('.');
  if (pos == std::string::npos)
    return {};
  return fileName.substr(pos + 1);
}

std::string mxp::utils::file::directory(const std::string& filePath) {
  auto pos = filePath.find_last_of(DIR_SEPARATOR);
  if (pos == std::string::npos) {
#ifdef HAVE_WIN32 
    pos = filePath.find_last_of('/');
    if (pos == std::string::npos)
      return {};
#else
    return {};
#endif
  }

  return filePath.substr(0, pos + 1);
}

std::string mxp::utils::file::parentDirectory(const std::string& path) {
  auto pos = path.find_last_of(DIR_SEPARATOR);
#ifdef HAVE_WIN32
  // If the \ appears right after the drive letter, or there's no backward /
  if ((pos == 2 && path[1] == ':') || pos == std::string::npos) {
    // Fall back to forward slashes
    pos = path.find_last_of('/');
  }
#endif
  if (pos == path.length() - 1) {
#ifdef HAVE_WIN32
    auto oldPos = pos;
#endif
    pos = path.find_last_of(DIR_SEPARATOR, pos - 1);
#ifdef HAVE_WIN32
    if ((pos == 2 && path[1] == ':') || pos == std::string::npos)
      pos = path.find_last_of('/', oldPos - 1);
#endif
  }
  return path.substr(0, pos + 1);
}

std::string mxp::utils::file::fileName(const std::string& filePath) {
  auto pos = filePath.find_last_of(DIR_SEPARATOR);
#ifdef HAVE_WIN32
  if (pos == std::string::npos)
    pos = filePath.find_last_of('/');
#endif
  if (pos == std::string::npos)
    return filePath;
  return filePath.substr(pos + 1);
}

std::string mxp::utils::file::firstFolder(const std::string& path) {
  size_t offset = 0;
  while (path[offset] == DIR_SEPARATOR
#ifdef HAVE_WIN32
      || path[offset] == '/'
#endif
     )
    offset++;
  auto pos = path.find_first_of(DIR_SEPARATOR, offset);
#ifdef HAVE_WIN32
  if (pos == std::string::npos)
    pos = path.find_first_of('/', offset);
#endif
  if (pos == std::string::npos)
    return {};
  return path.substr(offset, pos - offset);
}

std::string mxp::utils::file::removePath(const std::string& fullPath, const std::string& toRemove) {
  if (toRemove.length() == 0)
    return fullPath;
  auto pos = fullPath.find(toRemove) + toRemove.length();
  while (fullPath[pos] == DIR_SEPARATOR
#ifdef HAVE_WIN32
        || fullPath[pos] == '/'
#endif
     )
    pos++;
  if (pos >= fullPath.length())
    return {};
  return fullPath.substr(pos);
}

std::string& mxp::utils::file::toFolderPath(std::string& path) {
  if (path.size()>0 &&*path.crbegin() != DIR_SEPARATOR
#ifdef HAVE_WIN32
      && *path.crbegin() != '/'
#endif
    )
    path += DIR_SEPARATOR;
  return path;
}

std::string mxp::utils::file::toFolderPath(const std::string& path) {
  auto p = path;
  if (p.size() > 0 && *p.crbegin() != DIR_SEPARATOR
#ifdef HAVE_WIN32
      && *p.crbegin() != '/'
#endif
    )
    p += DIR_SEPARATOR;
  return p;
}

std::string mxp::utils::file::toAbsolutePath(const std::string& path) {
#ifdef HAVE_WIN32
  TCHAR buff[MAX_PATH];
  auto wpath = utils::ToWide(path.c_str());
  if (::GetFullPathName(wpath.get(), MAX_PATH, buff, nullptr) == 0) {
    throw std::runtime_error("Failed to convert " + path + " to absolute path (" + std::to_string(GetLastError()) + ")");
  }

  std::string res = utils::FromWide(buff).get();
#else
  char abs[PATH_MAX];
  if (realpath(path.c_str(), abs) == nullptr)
    throw std::system_error(errno, std::generic_category(), "Failed to convert to absolute path");

  std::string res = abs;
#endif

  return std::string{ res };
}

bool mxp::utils::file::isExistingDirectory(const std::string& path) {
#ifdef HAVE_WIN32
  auto wpath = utils::ToWide(path.c_str());
  auto dwAttrib = ::GetFileAttributes(wpath.get());
  return ((dwAttrib != INVALID_FILE_ATTRIBUTES) && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
#else
  // TODO
  return false;
#endif
}

bool mxp::utils::file::createDirectory(const std::string& path) {
#ifdef HAVE_WIN32
  auto wpath = utils::ToWide(path.c_str());
  if (::CreateDirectory(wpath.get(), nullptr)) {
    return true;
  } else if (ERROR_ALREADY_EXISTS == GetLastError()) {
    return true;
  }
#else
  const int ret = mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
  if (ret != -1) {
    return true;
  }
#endif
  return false;
}

std::string mxp::utils::file::concatPath(const mstring& a, const mstring& b) {
  return toFolderPath(a) + b;
}