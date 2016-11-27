/*****************************************************************************
* Media Explorer
*****************************************************************************/

#ifndef FS_IDEVICE_H
#define FS_IDEVICE_H

#include "mediaexplorer/Common.h"

namespace mxp {
namespace fs {

class IDevice {
public:
    virtual ~IDevice() = default;

    virtual const std::string& uuid() const = 0;

    virtual bool IsRemovable() const = 0;

    virtual bool isPresent() const = 0;

    virtual const std::string& mountpoint() const = 0;
};

} /* namespace fs */
} /* namespace mxp */

#endif /* FS_IDEVICE_H */
