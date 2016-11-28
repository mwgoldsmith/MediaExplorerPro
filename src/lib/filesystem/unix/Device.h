/*****************************************************************************
* Media Explorer
*****************************************************************************/

#ifndef FS_DEVICE_H
#define FS_DEVICE_H

#if !defined(HAVE_WIN32)

#include "filesystem/common/CommonDevice.h"

namespace mxp {
namespace fs {

class Device : public CommonDevice {
public:
    Device( const std::string& uuid, const std::string& mountpoint, bool isRemovable );
};

} /* namespace fs */
} /* namespace mxp */

#endif /* HAVE_WIN32 */

#endif /* FS_DEVICE_H */
