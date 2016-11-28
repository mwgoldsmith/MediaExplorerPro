/*****************************************************************************
* Media Explorer
*****************************************************************************/

#ifndef CHARSETS_H
#define CHARSETS_H

#include "stdafx.h"
#if HAVE_CONFIG_H
# include "config.h"
#endif

#ifdef HAVE_WIN32

namespace mxp {
namespace utils {

static inline std::unique_ptr<char[]> FromWide(const wchar_t *wide) {
  size_t len = ::WideCharToMultiByte(CP_UTF8, 0, wide, -1, nullptr, 0, nullptr, nullptr);
  if (len == 0)
    return nullptr;

  auto out = new char[len];
  ::WideCharToMultiByte(CP_UTF8, 0, wide, -1, out, static_cast<int>(len), nullptr, nullptr);
  return std::unique_ptr<char[]>(out);
}

static inline std::unique_ptr<wchar_t[]> ToWide(const char *utf8) {
  size_t len = ::MultiByteToWideChar(CP_UTF8, 0, utf8, -1, nullptr, 0);
  if (len == 0)
    return nullptr;

  auto out = new wchar_t[len];
  ::MultiByteToWideChar(CP_UTF8, 0, utf8, -1, out, static_cast<int>(len));
  return std::unique_ptr<wchar_t[]>(out);
}

} /* namespace utils */
} /* namespace mxp */

#endif /* HAVE_WIN32 */

#endif /* CHARSETS_H */

