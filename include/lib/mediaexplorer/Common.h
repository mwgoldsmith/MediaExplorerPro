/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#ifndef MXP_TYPES_H
#define MXP_TYPES_H

#include "stdafx.h"
#if HAVE_CONFIG_H
#  include "config.h"
#endif
#if defined(_MSC_VER)
#  include <sal.h>
#endif

/*****************************************************************************
 * Compilers definitions
 *****************************************************************************/
/* Helper for GCC version checks */
#ifdef __GNUC__
#  define MXP_GCC_VERSION(maj,min) \
     ((__GNUC__ > (maj)) || (__GNUC__ == (maj) && __GNUC_MINOR__ >= (min)))
#else
#  define MXP_GCC_VERSION(maj,min) (0)
#endif

/* Try to fix format strings for all versions of mingw and mingw64 */
#if defined(_WIN32) && defined(__USE_MINGW_ANSI_STDIO)
#  undef  PRId64
#  define PRId64 "lld"
#  undef  PRIi64
#  define PRIi64 "lli"
#  undef  PRIu64
#  define PRIu64 "llu"
#  undef  PRIo64
#  define PRIo64 "llo"
#  undef  PRIx64
#  define PRIx64 "llx"
#  define snprintf  __mingw_snprintf
#  define vsnprintf __mingw_vsnprintf
#  define swprintf  _snwprintf
#endif
#ifdef _MSC_VER
#  define strcasecmp _stricmp
#endif

/* Function attributes for compiler warnings */
#ifdef __GNUC__
#  define MXP_DEPRECATED      __attribute__((deprecated))
#  define MXP_NORETURN        __attribute__ ((noreturn))
#  if MXP_GCC_VERSION(3,4)
#    define MXP_USED          __attribute__ ((warn_unused_result))
#  else
#    define MXP_USED
#  endif
#  if defined(_WIN32) && MXP_GCC_VERSION(4,4)
#    define MXP_FORMAT(x,y)   __attribute__ ((format(gnu_printf,x,y)))
#  else
#    define MXP_FORMAT(x,y)   __attribute__ ((format(printf,x,y)))
#  endif
#  define MXP_FORMAT_POS(x)   __attribute__ ((format_arg(x)))
#  define MXP_FORMAT_ARG(x)   x
#  define MXP_MALLOC          __attribute__ ((malloc))
#elif defined(_MSC_VER)
#  if _MSC_VER >= 1310
#    define MXP_DEPRECATED    __declspec(deprecated)
#    define MXP_NORETURN      __declspec(noreturn)
#  else
#    define MXP_DEPRECATED
#    define MXP_NORETURN
#  endif
#  if _MSC_VER >= 1700
#    define MXP_USED          _Check_return_
#  else
#    define MXP_USED
#  endif
#  define MXP_FORMAT(x,y)
#  define MXP_FORMAT_POS(x)
#  if _MSC_VER > 1400
#    define MXP_FORMAT_ARG(p) _Printf_format_string_ p
#  else
#    define MXP_FORMAT_ARG(p) __format_string p
#  endif
#  define MXP_MALLOC
#else
#  define MXP_DEPRECATED
#  define MXP_NORETURN
#  define MXP_USED
#  define MXP_FORMAT(x,y)
#  define MXP_FORMAT_ARG(x)   x
#  define MXP_FORMAT_POS(x)
#  define MXP_MALLOC
#endif

 /* Branch prediction */
#ifdef __GNUC__
#  define MXP_LIKELY(p)       __builtin_expect(!!(p), 1)
#  define MXP_UNLIKELY(p)     __builtin_expect(!!(p), 0)
#  define MXP_UNREACHABLE()   __builtin_unreachable()
#else
#  define MXP_LIKELY(p)       (!!(p))
#  define MXP_UNLIKELY(p)     (!!(p))
#  define MXP_UNREACHABLE()   ((void)0)
#endif

#define MXP_IS_EMPTY_STR(str) (!str || !*str)
#define MXP_UNUSED(x)         (void)(x)

#if defined(_MSC_VER) && ! defined(MXP_STATIC)
#  ifdef MXP_EXPORTS 
#    define MXPAPI __declspec(dllexport)
#  else
#    define MXPAPI __declspec(dllimport)
#  endif
#else
#  define MXPAPI
#endif

#ifdef _MSC_VER
#  define THREAD_LOCAL __declspec(thread)
#else
#  ifndef HAVE_THREAD_LOCAL
#    define THREAD_LOCAL __thread
#  else
#    define THREAD_LOCAL thread_local
#  endif
#endif

//########
// Definition of portable char and string types
#ifndef USE_WCHAR
#define __MTEXT(x) x
using mstring = std::string;
using mchar = char;
#else 
#define __MTEXT(x) L##x
using mstring = std::wstring;
using mchar = wchar_t;
#endif

#define MTEXT(x) __MTEXT(x)

namespace mxp {

//########
// Conversion functions for portable strings
template<class T>
inline mstring to_mstring(const T);

#ifndef USE_WCHAR

template<>
inline mstring to_mstring(const std::wstring& wstr) {
  using converter = std::codecvt_utf8<wchar_t>;
  return std::wstring_convert<converter, wchar_t>().to_bytes(wstr);
}

template<>
inline mstring to_mstring(const std::string& str) { return str; }
template<>
inline mstring to_mstring(const char* str) { return mstring(str); }
template<>
inline mstring to_mstring(const wchar_t* wstr) { return to_mstring(wstr); }
template<>
inline mstring to_mstring(char* str) { return mstring(str); }
template<>
inline mstring to_mstring(wchar_t* wstr) { return to_mstring(wstr); }
template<>
inline mstring to_mstring(int value) { return std::to_string(value); }
template<>
inline mstring to_mstring(long value) { return std::to_string(value); }
template<>
inline mstring to_mstring(long long value) { return std::to_string(value); }
template<>
inline mstring to_mstring(unsigned int value) { return std::to_string(value); }
template<>
inline mstring to_mstring(unsigned long value) { return std::to_string(value); }
template<>
inline mstring to_mstring(unsigned long long value) { return std::to_string(value); }

#else /* USE_WCHAR */

template<>
inline mstring to_mstring(const std::string& str) {
  using converter = std::codecvt_utf8<wchar_t>;
  return std::wstring_convert<converter, wchar_t>().from_bytes(str);
}

template<>
inline mstring to_mstring(const std::wstring& wstr) { return wstr; }
template<>
inline mstring to_mstring(const char* str) { return to_mstring(str); }
template<>
inline mstring to_mstring(const wchar_t* wstr) { return mstring(wstr); }
template<>
inline mstring to_mstring(char* str) { return to_mstring(str); }
template<>
inline mstring to_mstring(wchar_t* wstr) { return mstring(wstr); }
template<>
inline mstring to_mstring(int value) { return std::to_wstring(value); }
template<>
inline mstring to_mstring(long value) { return std::to_wstring(value); }
template<>
inline mstring to_mstring(long long value) { return std::to_wstring(value); }
template<>
inline mstring to_mstring(unsigned int value) { return std::to_wstring(value); }
template<>
inline mstring to_mstring(unsigned long value) { return std::to_wstring(value); }
template<>
inline mstring to_mstring(unsigned long long value) { return std::to_wstring(value); }

#endif /* USE_WCHAR */

//########
// Forward references

class IArtist;
class IAlbum;
class IAlbumTrack;
class IAudioTrack;
class ICodec;
class IContainer;
class IDeviceLister;
class IDeviceListerCb;
class IGenre;
class IHistoryEntry;
class ILabel;
class ILogger;
class ILibraryFolder;
class IMedia;
class IMovie;
class IMediaExplorer;
class IMediaExplorerCb;
class IMediaDevice;
class IMediaFile;
class IMediaFolder;
class IMetadata;
class IPlaylist;
class ISettings;
class IShow;
class IShowEpisode;
class IStream;
class IVideoTrack;
enum class MediaType : int8_t;

namespace factory {

class IFileSystem;

} /* namespace factory */

namespace fs {

class IDevice;
class IDirectory;
class IFile;

} /* namespace fs */

using ArtistPtr        = std::shared_ptr<IArtist>;
using AlbumPtr         = std::shared_ptr<IAlbum>;
using AlbumTrackPtr    = std::shared_ptr<IAlbumTrack>;
using AudioTrackPtr    = std::shared_ptr<IAudioTrack>;
using CodecPtr         = std::shared_ptr<ICodec>;
using ContainerPtr     = std::shared_ptr<IContainer>;
using DeviceListerPtr  = std::shared_ptr<IDeviceLister>;
using FileSystemPtr    = std::shared_ptr<factory::IFileSystem>;
using DevicePtr        = std::shared_ptr<fs::IDevice>;
using DirectoryPtr     = std::shared_ptr<fs::IDirectory>;
using FilePtr          = std::shared_ptr<fs::IFile>;
using GenrePtr         = std::shared_ptr<IGenre>;
using HistoryPtr       = std::shared_ptr<IHistoryEntry>;
using LabelPtr         = std::shared_ptr<ILabel>;
using LibraryFolderPtr = std::shared_ptr<ILibraryFolder>;
using MoviePtr         = std::shared_ptr<IMovie>;
using MediaPtr         = std::shared_ptr<IMedia>;
using MediaDevicePtr   = std::shared_ptr<IMediaDevice>;
using MediaFolderPtr   = std::shared_ptr<IMediaFolder>;
using MediaFilePtr     = std::shared_ptr<IMediaFile>;
using MetadataPtr      = std::shared_ptr<IMetadata>;
using PlaylistPtr      = std::shared_ptr<IPlaylist>;
using SettingsPtr      = std::shared_ptr<ISettings>;
using ShowPtr          = std::shared_ptr<IShow>; 
using ShowEpisodePtr   = std::shared_ptr<IShowEpisode>;
using StreamPtr        = std::shared_ptr<IStream>;
using VideoTrackPtr    = std::shared_ptr<IVideoTrack>;

} /* namespace mxp */

#endif /* MXP_TYPES_H */
