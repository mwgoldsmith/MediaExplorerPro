/*****************************************************************************
* Media Explorer
*****************************************************************************/

#ifndef MEDIACONTAINER_H
#define MEDIACONTAINER_H

#include <string>
#include <string>

namespace mxp {
namespace parser {

struct MediaContainer {
  MediaContainer() 
    : Name({})
    , LongName({})
    , Extensions({}) {
  }

  MediaContainer(const char* name, const char* longName, const char* extensions)
    : Name(name)
    , LongName(longName)
    , Extensions(extensions) {
  }

  const std::string Name;
  const std::string LongName;
  const std::string Extensions;
};

} /* namespace parser */
} /* namespace mxp */

#endif /* MEDIACONTAINER_H */
