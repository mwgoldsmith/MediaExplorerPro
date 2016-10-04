/*****************************************************************************
* Media Explorer
*****************************************************************************/

#ifndef MEDIATYPE_H
#define MEDIATYPE_H

namespace mxp {
namespace parser {
 
enum class MediaType {
  None = -1,
  Audio,
  Image,
  Subtitles,
  Video
};

} /* namespace parser */
} /* namespace mxp */

#endif /* MEDIATYPE_H */
