/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#ifndef MXP_TYPES_H
#define MXP_TYPES_H

#if HAVE_CONFIG_H
# include "config.h"
#endif

#include <algorithm>
#include <atomic>
#include <cassert>
#include <cerrno>
#include <chrono>
#include <cinttypes>
#include <codecvt>
#include <condition_variable>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <exception>
#include <functional>
#include <iostream>
#include <locale>
#include <memory>
#include <mutex>
#include <queue>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>
#include <tuple>
#include <unordered_map>
#include <vector>
#include <time.h>

#ifndef _WIN32
#include <dirent.h>
#include <limits.h>
#include <sys/stat.h>
#include <unistd.h>
#include <system_error>
#endif

//########
// Definition of portable char and string types
#ifdef USE_CHAR
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

#ifdef USE_CHAR

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

#else /* USE_CHAR */

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

#endif /* USE_CHAR */

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
class IMedia;
class IMovie;
class IMediaExplorer;
class IMediaExplorerCb;
class IMediaDevice;
class IMediaFile;
class IMediaFolder;
class IMetadata;
class IPlaylist;
class IShow;
class IShowEpisode;
class IStream;
class IVideoTrack;

namespace factory {

class IFileSystem;

} /* namespace factory */

namespace fs {

class IDevice;
class IDirectory;
class IFile;

} /* namespace fs */

using ArtistPtr       = std::shared_ptr<IArtist>;
using AlbumPtr        = std::shared_ptr<IAlbum>;
using AlbumTrackPtr   = std::shared_ptr<IAlbumTrack>;
using AudioTrackPtr   = std::shared_ptr<IAudioTrack>;
using CodecPtr        = std::shared_ptr<ICodec>;
using ContainerPtr    = std::shared_ptr<IContainer>;
using DeviceListerPtr = std::shared_ptr<IDeviceLister>;
using FileSystemPtr   = std::shared_ptr<factory::IFileSystem>;
using DevicePtr       = std::shared_ptr<fs::IDevice>;
using DirectoryPtr    = std::shared_ptr<fs::IDirectory>;
using FilePtr         = std::shared_ptr<fs::IFile>;
using GenrePtr        = std::shared_ptr<IGenre>;
using HistoryPtr      = std::shared_ptr<IHistoryEntry>;
using LabelPtr        = std::shared_ptr<ILabel>;
using MoviePtr        = std::shared_ptr<IMovie>;
using MediaPtr        = std::shared_ptr<IMedia>;
using MediaDevicePtr  = std::shared_ptr<IMediaDevice>;
using MediaFolderPtr  = std::shared_ptr<IMediaFolder>;
using MediaFilePtr    = std::shared_ptr<IMediaFile>;
using MetadataPtr     = std::shared_ptr<IMetadata>;
using PlaylistPtr     = std::shared_ptr<IPlaylist>;
using ShowPtr         = std::shared_ptr<IShow>; 
using ShowEpisodePtr  = std::shared_ptr<IShowEpisode>;
using StreamPtr       = std::shared_ptr<IStream>;
using VideoTrackPtr   = std::shared_ptr<IVideoTrack>;

} /* namespace mxp */

#endif /* MXP_TYPES_H */
