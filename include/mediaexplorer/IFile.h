/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#ifndef MXP_IFILE_H
#define MXP_IFILE_H

#include <string>

namespace mxp {

class IFile {
public:
  enum class Type {
    /// Unknown type, so far
    Unknown,
    /// The file is the entire media.
    /// This implies there's only a single file for a media
    Entire,
    /// The main file of a media.
    /// This implies the media has other parts, for instance, the "Main"
    /// part is the video plus some default soundtrack, and there is
    /// another file of type "soundtrack"
    Main,
    /// A part of a media (for instance, the first half of a movie)
    Part,
    /// External soundtrack
    Soundtrack,
    /// External subtitles
    Subtitles,
  };

  virtual ~IFile() = default;
  virtual int64_t id() const = 0;
  virtual const std::string& mrl() const = 0;
  virtual Type type() const = 0;
  virtual unsigned int lastModificationDate() const = 0;
};

} /* namespace mxp */

#endif /* MXP_IFILE_H */
