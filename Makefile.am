ACLOCAL_AMFLAGS = -I m4

AM_CPPFLAGS = -Wall -Wsign-compare -Wextra -Wstrict-aliasing -Wstrict-overflow \
			  -Wformat=2 -Wno-unused-parameter -Wcast-align -Wpointer-arith \
			  -Wwrite-strings \
			  -pipe

MEDIALIB_CPPFLAGS = $(AM_CPPFLAGS) -I$(top_srcdir)/include -I$(top_srcdir)/src

libmediaexplorer_ladir = $(includedir)/mediaexplorer

libmediaexplorer_la_HEADERS = \
	include/mediaexplorer/IAudioTrack.h \
	include/mediaexplorer/IDeviceLister.h \
	include/mediaexplorer/IDeviceListerCb.h \
	include/mediaexplorer/IFile.h \
	include/mediaexplorer/ILabel.h \
	include/mediaexplorer/ILogger.h \
	include/mediaexplorer/IMedia.h \
	include/mediaexplorer/IMediaExplorer.h \
	include/mediaexplorer/IMediaExplorerCb.h \
	include/mediaexplorer/ISettings.h \
	include/mediaexplorer/IPlaylist.h \
	include/mediaexplorer/IVideoTrack.h \
	include/mediaexplorer/LogLevel.h \
	include/mediaexplorer/MediaSearchAggregate.h \
	include/mediaexplorer/SearchAggregate.h \
	include/mediaexplorer/SortingCriteria.h \
	include/mediaexplorer/Types.h \
	$(NULL)

libmediaexplorer_la_SOURCES = \
	src/discoverer/DiscovererWorker.cpp \
	src/discoverer/FsDiscoverer.cpp \
	src/database/SqliteConnection.cpp \
	src/database/SqliteTools.cpp \
	src/database/SqliteTransaction.cpp \
	src/factory/DeviceListerFactory.cpp \
	src/factory/FileSystemFactory.cpp \
	src/filesystem/common/CommonDevice.cpp \
	src/filesystem/common/CommonDirectory.cpp \
	src/filesystem/common/CommonFile.cpp \
	src/filesystem/unix/File.cpp \
	src/logging/IostreamLogger.cpp \
	src/logging/Logger.cpp \
	src/metadata_services/MetadataParser.cpp \
	src/parser/Parser.cpp \
	src/parser/ParserService.cpp \
	src/utils/Filename.cpp \
	src/utils/ModificationsNotifier.cpp \
	src/AudioTrack.cpp \
	src/Device.cpp \
	src/File.cpp \
	src/Folder.cpp \
	src/Label.cpp \
	src/Media.cpp \
	src/MediaExplorer.cpp \
	src/Playlist.cpp \
	src/Settings.cpp \
	src/VideoTrack.cpp \
	$(NULL)

noinst_HEADERS = \
	include/discoverer/IDiscoverer.h \
	include/factory/IFileSystem.h \
	include/filesystem/IDevice.h \
	include/filesystem/IDirectory.h \
	include/filesystem/IFile.h \
	include/mediaexplorer/IAudioTrack.h \
	include/mediaexplorer/IDeviceLister.h \
	include/mediaexplorer/IDeviceListerCb.h \
	include/mediaexplorer/IFile.h \
	include/mediaexplorer/ILabel.h \
	include/mediaexplorer/ILogger.h \
	include/mediaexplorer/IMedia.h \
	include/mediaexplorer/IMediaExplorer.h \
	include/mediaexplorer/IMediaExplorerCb.h \
	include/mediaexplorer/IPlaylist.h \
	include/mediaexplorer/ISettings.h \
	include/mediaexplorer/IVideoTrack.h \
	include/mediaexplorer/LogLevel.h \
	include/mediaexplorer/MediaSearchAggregate.h \
	include/mediaexplorer/SearchAggregate.h \
	include/mediaexplorer/SortingCriteria.h \
	include/mediaexplorer/Types.h \
	include/Fixup.h \
	include/Types.h \
	src/compat/ConditionVariable.h \
	src/compat/Mutex.h \
	src/compat/Thread.h \
	src/database/DatabaseHelpers.h \
	src/database/SqliteConnection.h \
	src/database/SqliteErrors.h \
	src/database/SqliteTools.h \
	src/database/SqliteTraits.h \
	src/database/SqliteTransaction.h \
	src/database/Statement.h \
	src/database/Row.h \
	src/discoverer/DiscovererWorker.h \
	src/discoverer/FsDiscoverer.h \
	src/factory/DeviceListerFactory.h \
	src/factory/FileSystemFactory.h \
	src/filesystem/common/CommonDevice.h \
	src/filesystem/common/CommonDirectory.h \
	src/filesystem/common/CommonFile.h \
	src/filesystem/unix/File.h \
	src/logging/IostreamLogger.h \
	src/logging/Logger.h \
	src/metadata_services/MetadataParser.h \
	src/parser/Parser.h \
	src/parser/ParserService.h \
	src/parser/Task.h \
	src/utils/Cache.h \
	src/utils/Filename.h \
	src/utils/ModificationsNotifier.h \
	src/utils/ReadLocker.h \
	src/utils/SWMRLock.h \
	src/utils/WriteLocker.h \
	src/AudioTrack.h \
	src/Device.h \
	src/File.h \
	src/Folder.h \
	src/Label.h \
	src/Media.h \
	src/MediaExplorer.h \
	src/Playlist.h \
	src/Settings.h \
	src/VideoTrack.h \
	$(NULL)

if HAVE_WIN32
libmediaexplorer_la_SOURCES += \
	src/filesystem/win32/Device.cpp \
	src/filesystem/win32/DeviceLister.cpp \
	src/filesystem/win32/Directory.cpp \
	$(NULL)
noinst_HEADERS += \
	src/filesystem/win32/Device.h \
	src/filesystem/win32/DeviceLister.h \
	src/filesystem/win32/Directory.h \
	src/utils/Charsets.h \
	$(NULL)
else
libmediaexplorer_la_SOURCES += \
	src/filesystem/unix/Directory.cpp \
	src/filesystem/unix/Device.cpp \
	$(NULL)
noinst_HEADERS += \
	src/filesystem/unix/Device.h \
	src/filesystem/unix/Directory.h \
	$(NULL)
if HAVE_LINUX
if !HAVE_ANDROID
libmediaexplorer_la_SOURCES += \
	src/filesystem/unix/DeviceLister.cpp \
	$(NULL)
noinst_HEADERS += \
	src/filesystem/unix/DeviceLister.h \
	$(NULL)
endif
endif
endif

libmediaexplorer_la_CPPFLAGS = \
	$(MEDIALIB_CPPFLAGS) \
	$(SQLITE_CFLAGS) \
	$(NULL)

libmediaexplorer_la_LIBADD = \
	$(SQLITE_LIBS) 			\
	$(NULL)

libmediaexplorer_la_LDFLAGS = -no-undefined -export-symbols-regex 'newMediaExplorer'
if HAVE_DARWIN
libmediaexplorer_la_LIBADD += libmediaexplorer_macos.la
libmediaexplorer_la_LDFLAGS += -Wl,-undefined,dynamic_lookup,-framework,Foundation
endif

if HAVE_LIBJPEG
libmediaexplorer_la_CPPFLAGS += $(LIBJPEG_CFLAGS)
libmediaexplorer_la_LIBADD += $(LIBJPEG_LIBS)
endif

lib_LTLIBRARIES = libmediaexplorer.la
if HAVE_DARWIN
lib_LTLIBRARIES += libmediaexplorer_macos.la
endif

bin_PROGRAMS = samples

samples_SOURCES = 						\
	tests/mxp.cpp 							\
	$(NULL)

samples_CPPFLAGS = 						\
	$(MEDIALIB_CPPFLAGS)				\
	-DSRC_DIR=\"$(top_srcdir)\"	\
	-I$(top_srcdir)/tests				\
	$(SQLITE_CFLAGS)						\
	$(NULL)

samples_LDADD = 							\
	libmediaexplorer.la					\
	$(SQLITE_LIBS)							\
	$(NULL)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = mediaexplorer.pc
EXTRA_DIST = mediaexplorer.pc
