/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#ifndef MXP_IMEDIAFILE_H
#define MXP_IMEDIAFILE_H

#include "mediaexplorer/Common.h"

namespace mxp {

enum class MediaFileType {
  // Unknown type, so far
  Unknown,
  // The file is the entire media.
  // This implies there's only a single file for a media
  Entire,
  // The main file of a media.
  // This implies the media has other parts, for instance, the "Main"
  // part is the video plus some default soundtrack, and there is
  // another file of type "soundtrack"
  Main,
  // A part of a media (for instance, the first half of a movie)
  Part,
  // External soundtrack
  Soundtrack,
  // External subtitles
  Subtitles,
};

class IMediaFile {
public:

  virtual ~IMediaFile() = default;

  virtual int64_t Id() const = 0;

  virtual const std::string& GetMrl() const = 0;

  virtual MediaFileType GetType() const = 0;

  virtual time_t LastModificationDate() const = 0;
};

} /* namespace mxp */

#endif /* MXP_IMEDIAFILE_H */
