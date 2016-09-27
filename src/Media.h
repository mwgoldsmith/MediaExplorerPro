/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#ifndef MEDIA_H
#define MEDIA_H

#include <sqlite3.h>

#include "mediaexplorer/IMedia.h"
#include "factory/IFileSystem.h"
#include "MediaFile.h"
#include "database/DatabaseHelpers.h"
#include "utils/Cache.h"

namespace mxp {

//class Album;
class MediaFolder;
//class ShowEpisode;
//class AlbumTrack;
class Media;

namespace policy {
struct MediaTable {
  static const std::string Name;
  static const std::string PrimaryKeyColumn;
  static int64_t Media::*const PrimaryKey;
};
}

class Media : public IMedia, public DatabaseHelpers<Media, policy::MediaTable> {
public:
  // Those should be private, however the standard states that the expression
  // ::new (pv) T(std::forward(args)...)
  // shall be well-formed, and private constructor would prevent that.
  // There might be a way with a user-defined allocator, but we'll see that later...
  Media(MediaExplorerPtr ml , sqlite::Row& row);
  Media(MediaExplorerPtr ml, const std::string &title, Type type);

  static std::shared_ptr<Media> create(MediaExplorerPtr ml, Type type, const fs::IFile& file);
  static bool createTable(DBConnection connection);
  static bool createTriggers(DBConnection connection);

  virtual int64_t id() const override;
  virtual Type type() override;
  virtual SubType subType() const override;
  void setType(Type type);
  virtual const std::string& title() const override;
  void setTitle(const std::string& title);
  //virtual AlbumTrackPtr albumTrack() const override;
  //void setAlbumTrack(AlbumTrackPtr albumTrack);
  virtual int64_t duration() const override;
  void setDuration(int64_t duration);
  //virtual ShowEpisodePtr showEpisode() const override;
  //void setShowEpisode(ShowEpisodePtr episode);
  virtual bool addLabel(LabelPtr label) override;
  virtual bool removeLabel(LabelPtr label) override;
  virtual std::vector<LabelPtr> labels() override;
  virtual int playCount() const  override;
  virtual bool increasePlayCount() override;
  virtual float progress() const override;
  virtual bool setProgress(float progress) override;
  virtual int rating() const override;
  virtual bool setRating(int rating) override;
  virtual bool isFavorite() const override;
  virtual bool setFavorite(bool favorite) override;
  virtual const std::vector<FilePtr>& files() const override;
  //virtual MoviePtr movie() const override;
  //void setMovie(MoviePtr movie);
  bool addVideoTrack(const std::string& codec, unsigned int width, unsigned int height, float fps, const std::string& language, const std::string& description);
  virtual std::vector<VideoTrackPtr> videoTracks() override;
  bool addAudioTrack(const std::string& codec, unsigned int bitrate, unsigned int sampleRate, unsigned int nbChannels, const std::string& language, const std::string& desc);
  virtual std::vector<AudioTrackPtr> audioTracks() override;
  virtual const std::string& thumbnail() override;
  virtual unsigned int insertionDate() const override;
  virtual unsigned int releaseDate() const override;
  void setReleaseDate(unsigned int date);
  void setThumbnail(const std::string& thumbnail);
  bool save();

  std::shared_ptr<MediaFile> addFile(const fs::IFile& fileFs, MediaFolder& parentFolder, fs::IDirectory& parentFolderFs , IFile::Type type);
  void removeFile(MediaFile& file);

  static std::vector<MediaPtr> listAll(MediaExplorerPtr ml, Type type , SortingCriteria sort, bool desc);
  static std::vector<MediaPtr> search(MediaExplorerPtr ml, const std::string& title);
  static std::vector<MediaPtr> fetchHistory(MediaExplorerPtr ml);
  static void clearHistory(MediaExplorerPtr ml);

private:
  MediaExplorerPtr m_ml;

  // DB fields:
  int64_t      m_id;
  Type         m_type;
  SubType      m_subType;
  int64_t      m_duration;
  unsigned int m_playCount;
  time_t       m_lastPlayedDate;
  float        m_progress;
  int          m_rating;
  unsigned int m_insertionDate;
  unsigned int m_releaseDate;
  std::string  m_thumbnail;
  std::string  m_title;
  // We store the filename as a shortcut when sorting. The filename (*not* the title
  // might be used as a fallback
  std::string  m_filename;
  bool         m_isFavorite;
  bool         m_isPresent;

  // Auto fetched related properties
  //mutable Cache<AlbumTrackPtr> m_albumTrack;
  //mutable Cache<ShowEpisodePtr> m_showEpisode;
  //mutable Cache<MoviePtr> m_movie;
  mutable Cache<std::vector<FilePtr>> m_files;
  bool m_changed;

  friend policy::MediaTable;
};

}
#endif // MEDIA_H
