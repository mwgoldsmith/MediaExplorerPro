/*****************************************************************************
* Media Explorer
*****************************************************************************/

#include "stdafx.h"
#if HAVE_CONFIG_H
# include "config.h"
#endif

#include "DeviceLister.h"
#include "logging/Logger.h"
#include "utils/Charsets.h"

namespace {
template <typename FUNC>
class UniqueHandle {
public:
  UniqueHandle(HANDLE h, FUNC* f) noexcept : m_handle(h), m_func(f) {}
  ~UniqueHandle() { m_func(m_handle); }
  UniqueHandle(const UniqueHandle&) = delete;
  UniqueHandle& operator=(const UniqueHandle&) = delete;
  UniqueHandle(UniqueHandle&&) = delete;
  UniqueHandle& operator=(UniqueHandle&&) = delete;

private:
  HANDLE m_handle;
  FUNC*  m_func;
};
}

std::vector<std::tuple<std::string, std::string, bool>> mxp::fs::DeviceLister::Devices() const {
  TCHAR volumeName[MAX_PATH];
  auto handle = ::FindFirstVolume(volumeName, sizeof(volumeName));
  if (handle == INVALID_HANDLE_VALUE) {
    std::stringstream ss;
    ss << "Failed to list devices (error code" << GetLastError() << ')';
    throw std::runtime_error(ss.str());
  }

  UniqueHandle<decltype(FindVolumeClose)> uh(handle, &FindVolumeClose);
  std::vector<std::tuple<std::string, std::string, bool>> res;
  for (auto success =  TRUE; ; success = ::FindNextVolume(handle, volumeName, sizeof(volumeName))) {
    if (success == FALSE) {
      auto err = GetLastError();
      if (err == ERROR_NO_MORE_FILES)
        break;
      std::stringstream ss;
      ss << "Failed to list devices (error code" << err << ')';
      throw std::runtime_error(ss.str());
    }

    auto lastChar = wcslen(volumeName) - 1;
    if (volumeName[0] != L'\\' || volumeName[1] != L'\\' || volumeName[2] != L'?' ||
       volumeName[3] != L'\\' || volumeName[lastChar] != L'\\')
      continue;

    TCHAR buffer[MAX_PATH + 1];
    DWORD buffLength = sizeof(buffer);

    if (::GetVolumePathNamesForVolumeName(volumeName, buffer, buffLength, &buffLength) == 0)
      continue;
    std::string mountpoint = utils::FromWide(buffer).get();

    // Filter out anything which isn't a removable or fixed drive. We don't care about network
    // drive here.
    auto type = ::GetDriveType(buffer);
    if (type != DRIVE_REMOVABLE && type != DRIVE_FIXED)
      continue;

    std::string uuid =  utils::FromWide(volumeName).get();

    LOG_INFO("Discovered device ", uuid, "; mounted on ", mountpoint, "; removable: ", type == DRIVE_REMOVABLE ? "yes" : "no");
    res.emplace_back(std::make_tuple(uuid, mountpoint, type == DRIVE_REMOVABLE));
  }
  return res;
}


