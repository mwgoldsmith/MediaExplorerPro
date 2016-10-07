/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#ifndef MEDIA_H
#define MEDIA_H

#include "factory/IFileSystem.h"
#include "MediaFile.h"

#include "database/DatabaseHelpers.h"
#include "mediaexplorer/IMedia.h"
#include "mediaexplorer/Types.h"
#include "utils/Cache.h"
#include "Types.h"

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

  virtual int64_t Id() const override;
  virtual Type GetType() override;
  virtual SubType GetSubType() const override;
  virtual const std::string& GetTitle() const override;
  //virtual AlbumTrackPtr albumTrack() const override;
  //void setAlbumTrack(AlbumTrackPtr albumTrack);
  virtual int64_t GetDuration() const override;
  //virtual ShowEpisodePtr showEpisode() const override;
  //void setShowEpisode(ShowEpisodePtr episode);
  virtual bool AddLabel(LabelPtr label) override;
  virtual bool RemoveLabel(LabelPtr label) override;
  virtual std::vector<LabelPtr> GetLabels() override;
  virtual int GetPlayCount() const  override;
  virtual bool IncreasePlayCount() override;
  virtual float GetProgress() const override;
  virtual bool SetProgress(float progress) override;
  virtual int Rating() const override;
  virtual bool SetRating(int rating) override;
  virtual bool IsFavorite() const override;
  virtual bool SetFavorite(bool favorite) override;
  virtual const std::vector<MediaFilePtr>& Files() const override;
  //virtual MoviePtr movie() const override;
  //void setMovie(MoviePtr movie);
  virtual std::vector<VideoTrackPtr> VideoTracks() override;
  virtual std::vector<AudioTrackPtr> AudioTracks() override;
  virtual const std::string& Thumbnail() override;
  virtual time_t InsertionDate() const override;
  virtual time_t ReleaseDate() const override;

  bool AddVideoTrack(const std::string& codec, unsigned int width, unsigned int height, float fps, const std::string& language, const std::string& description);
  bool AddAudioTrack(const std::string& codec, unsigned int bitrate, unsigned int sampleRate, unsigned int numChannels, const std::string& language, const std::string& desc);

  std::shared_ptr<MediaFile> AddFile(const fs::IFile& fileFs, MediaFolder& parentFolder, fs::IDirectory& parentFolderFs, mxp::IMediaFile::Type type);
  void RemoveFile(MediaFile& file);

  void SetType(Type type);
  void SetTitle(const std::string& title);
  void SetDuration(int64_t duration); 
  void SetReleaseDate(time_t date);
  void SetThumbnail(const std::string& thumbnail);
  bool Save();

  static bool CreateTable(DBConnection connection);
  static std::shared_ptr<Media> Create(MediaExplorerPtr ml, Type type, const fs::IFile& file); 
  static bool CreateTriggers(DBConnection connection);

  static std::vector<MediaPtr> Search(MediaExplorerPtr ml, const std::string& title);
  static std::vector<MediaPtr> ListAll(MediaExplorerPtr ml, Type type , SortingCriteria sort, bool desc);

  static std::vector<MediaPtr> FetchHistory(MediaExplorerPtr ml);
  static void ClearHistory(MediaExplorerPtr ml);

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
  time_t       m_insertionDate;
  time_t       m_releaseDate;
  std::string  m_thumbnail;
  std::string  m_title;
  // We store the filename as a shortcut when sorting. The filename (*not* the title
  // might be used as a fallback
  std::string  m_filename;
  bool         m_isFavorite;
  bool         m_isPresent;
  bool         m_changed;

  // Auto fetched related properties
  //mutable Cache<AlbumTrackPtr> m_albumTrack;
  //mutable Cache<ShowEpisodePtr> m_showEpisode;
  //mutable Cache<MoviePtr> m_movie;
  mutable Cache<std::vector<MediaFilePtr>> m_files;

  friend struct policy::MediaTable;
};

}
#endif // MEDIA_H
