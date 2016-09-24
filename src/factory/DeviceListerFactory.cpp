/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#if HAVE_CONFIG_H
# include "config.h"
#endif

#include "DeviceListerFactory.h"

#if defined(__linux__) && !defined(__ANDROID__)
#include "filesystem/unix/DeviceLister.h"
#elif defined(_WIN32)
#include "filesystem/win32/DeviceLister.h"
#endif

mxp::DeviceListerPtr mxp::factory::createDeviceLister() {
#if (defined(__linux__) && !defined(__ANDROID__)) || defined(_WIN32)
  return std::make_shared<fs::DeviceLister>();
#endif
  return nullptr;
}
