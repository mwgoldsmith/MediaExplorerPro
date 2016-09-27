/*****************************************************************************
* Media Explorer
*****************************************************************************/

#pragma once

#include "filesystem/common/CommonDevice.h"

namespace mxp {
namespace fs {

class Device : public CommonDevice {
public:
    Device(const std::string& uuid, const std::string& mountpoint, bool isRemovable);
};

}
}
