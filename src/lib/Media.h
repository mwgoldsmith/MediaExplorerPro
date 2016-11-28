/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#ifndef MEDIA_H
#define MEDIA_H

#include "database/DatabaseHelpers.h"
#include "mediaexplorer/IMedia.h"
#include "mediaexplorer/IMediaFile.h"
#include "mediaexplorer/Common.h"
#include "utils/Cache.h"
#include "Types.h"

namespace mxp {

class Album;
class MediaFolder;
class ShowEpisode;
class AlbumTrack;
class Media;
class MediaFile;

namespace policy {
struct MediaTable {
  static const mstring Name;
  static const mstring PrimaryKeyColumn;
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

  Media(MediaExplorerPtr ml, const mstring &title, MediaType type);

  virtual int64_t Id() const override;

  virtual MediaType GetType() override;

  virtual MediaSubType GetSubType() const override;

  virtual const mstring& GetTitle() const override;

  virtual AlbumTrackPtr AlbumTrack() const override;

  void SetAlbumTrack(AlbumTrackPtr albumTrack);

  virtual int64_t GetDuration() const override;

  virtual ShowEpisodePtr ShowEpisode() const override;

  void SetShowEpisode(ShowEpisodePtr episode);

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

  virtual MoviePtr Movie() const override;

  void SetMovie(MoviePtr movie);

  virtual std::vector<VideoTrackPtr> VideoTracks() override;

  virtual std::vector<AudioTrackPtr> AudioTracks() override;

  virtual const mstring& Thumbnail() override;

  virtual time_t InsertionDate() const override;

  virtual time_t ReleaseDate() const override;

  bool AddVideoTrack(const mstring& codec, unsigned int width, unsigned int height, float fps, const mstring& language, const mstring& description);

  bool AddAudioTrack(const mstring& codec, unsigned int bitrate, unsigned int sampleRate, unsigned int numChannels, const mstring& language, const mstring& desc);

  std::shared_ptr<MediaFile> AddFile(const fs::IFile& fileFs, MediaFolder& parentFolder, fs::IDirectory& parentFolderFs, MediaFileType type);

  void RemoveFile(MediaFile& file);

  void SetType(MediaType type);

  void SetTitle(const mstring& title);

  virtual void SetDuration(int64_t duration) override;

  virtual ContainerPtr GetContainer() const override;

  virtual void SetContainer(ContainerPtr container) override;

  void SetReleaseDate(time_t date);

  void SetThumbnail(const mstring& thumbnail);

  bool Save();

  virtual std::vector<MetadataPtr> GetMetadata() const override;

  virtual bool AddMetadata(int64_t metadataId) override;

  virtual bool RemoveMetadata(int64_t metadataId) override;

  static std::shared_ptr<Media> Create(MediaExplorerPtr ml, MediaType type, const fs::IFile& file) noexcept;

  static bool CreateTable(DBConnection connection) noexcept;

  static bool CreateTriggers(DBConnection connection) noexcept;

  static std::vector<MediaPtr> Search(MediaExplorerPtr ml, const mstring& title);

  static std::vector<MediaPtr> ListAll(MediaExplorerPtr ml, SortingCriteria sort, bool desc);

  static std::vector<MediaPtr> ListAll(MediaExplorerPtr ml, MediaType type, SortingCriteria sort, bool desc);

  static std::vector<MediaPtr> FetchHistory(MediaExplorerPtr ml);

  static void ClearHistory(MediaExplorerPtr ml);

private:
  static bool Create(DBConnection connection, const mstring& sql, const mstring& type, const mstring& name) noexcept;

public:
  const utils::Guid& GetGuid() const override;
private:
  MediaExplorerPtr m_ml;

  // DB fields:
  int64_t      m_id;
  MediaType    m_type;
  MediaSubType m_subType;
  utils::Guid  m_guid;
  int64_t      m_containerId;
  int64_t      m_duration;
  unsigned int m_playCount;
  time_t       m_lastPlayedDate;
  float        m_progress;
  int          m_rating;
  time_t       m_insertionDate;
  time_t       m_releaseDate;
  mstring      m_thumbnail;
  mstring      m_title;
  // We store the filename as a shortcut when sorting. The filename (*not* the title
  // might be used as a fallback
  mstring      m_filename;
  bool         m_isFavorite;
  bool         m_isPresent;
  bool         m_changed;

  // Auto fetched related properties
  mutable Cache<AlbumTrackPtr> m_albumTrack;
  mutable Cache<ShowEpisodePtr> m_showEpisode;
  mutable Cache<MoviePtr> m_movie;
  mutable Cache<ContainerPtr> m_container;
  mutable Cache<std::vector<MediaFilePtr>> m_files;

  friend struct policy::MediaTable;
};

}
#endif // MEDIA_H
