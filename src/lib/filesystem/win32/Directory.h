/*****************************************************************************
* Media Explorer
*****************************************************************************/

#pragma once

#include "stdafx.h"
#include "filesystem/common/CommonDirectory.h"

namespace mxp {
namespace fs {

class Directory : public CommonDirectory {
public:
  Directory(const std::string& path, factory::IFileSystem& fsFactory);

private:
  virtual void read() const override;
};

}
}
