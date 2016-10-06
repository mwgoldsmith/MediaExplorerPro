/*****************************************************************************
* Media Explorer
*****************************************************************************/

#ifndef AVTYPE_H
#define AVTYPE_H

#include <string>

namespace mxp {
namespace av {
 
enum class AvType {
  None = -1,
  Audio,
  Image,
  Subtitles,
  Video
};

AvType GetAvType(int libAvType);
std::string GetAvTypeString(AvType type);

} /* namespace av */
} /* namespace mxp */

#endif /* AVTYPE_H */
