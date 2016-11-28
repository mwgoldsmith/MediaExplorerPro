/*****************************************************************************
* Media Explorer
*****************************************************************************/

#ifndef MXP_MEDIATYPE_H
#define MXP_MEDIATYPE_H

namespace mxp {

enum class MediaType : int8_t {
  Unknown = -1,

  None = 0,

  Audio,

  Image,

  Subtitles,

  Video
};

enum class MediaSubType : int8_t {
  Unknown,

  ShowEpisode,

  Movie,

  AlbumTrack
};

} /* namespace mxp */

#endif /* MXP_MEDIATYPE_H */
