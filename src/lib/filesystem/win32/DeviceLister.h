/*****************************************************************************
* Media Explorer
*****************************************************************************/

#pragma once

#include "stdafx.h"
#include "mediaexplorer/IDeviceLister.h"

namespace mxp {
namespace fs {

class DeviceLister : public IDeviceLister {
public:
  virtual std::vector<std::tuple<std::string, std::string, bool>> Devices() const override;
};

}
}
