/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#ifndef MXP_IMEDIAFILE_H
#define MXP_IMEDIAFILE_H

#include <string>
#include <stdint.h>

namespace mxp {

class IMediaFile {
public:
  enum class Type {
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

  virtual ~IMediaFile() = default;
  virtual int64_t Id() const = 0;
  virtual const std::string& mrl() const = 0;
  virtual Type type() const = 0;
  virtual time_t LastModificationDate() const = 0;
};

} /* namespace mxp */

#endif /* MXP_IMEDIAFILE_H */
