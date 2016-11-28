/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#include "stdafx.h"
#if HAVE_CONFIG_H
# include "config.h"
#endif

#include "DeviceListerFactory.h"

#if defined(HAVE_LINUX) && !defined(HAVE_ANDROID)
#  include "filesystem/unix/DeviceLister.h"
#elif defined(HAVE_WIN32)
#  include "filesystem/win32/DeviceLister.h"
#endif

mxp::DeviceListerPtr mxp::factory::createDeviceLister() {
#if (defined(HAVE_LINUX) && !defined(HAVE_ANDROID)) || defined(HAVE_WIN32)
  return std::make_shared<fs::DeviceLister>();
#else
  return nullptr;
#endif
}
