/*****************************************************************************
* Media Explorer
*****************************************************************************/

#ifndef MXP_MEDIATYPE_H
#define MXP_MEDIATYPE_H

namespace mxp {

enum class MediaType {
  Unknown = -1,

  None = 0,

  Audio,

  Image,

  Subtitles,

  Video
};

} /* namespace mxp */

#endif /* MXP_MEDIATYPE_H */
