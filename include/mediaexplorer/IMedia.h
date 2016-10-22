/*****************************************************************************
* Media Explorer
*****************************************************************************/

#ifndef MXP_IMEDIA_H
#define MXP_IMEDIA_H

#include "mediaexplorer/Common.h"

namespace mxp {

class IMedia {
public:
  enum class Type : uint8_t {
    VideoType,
    AudioType,
    UnknownType
  };

  enum class SubType : uint8_t {
    Unknown,
    ShowEpisode,
    Movie,
    AlbumTrack,
  };

  virtual ~IMedia() = default;

  virtual int64_t Id() const = 0;

  virtual Type GetType() = 0;

  virtual SubType GetSubType() const = 0;

  virtual void SetDuration(int64_t duration) = 0;

  virtual ContainerPtr GetContainer() const = 0;

  virtual void SetContainer(ContainerPtr container) = 0;

  virtual const mstring& GetTitle() const = 0;

  virtual AlbumTrackPtr AlbumTrack() const = 0;

  /**
   * @brief duration Returns the media duration in ms
   */
  virtual int64_t GetDuration() const = 0;

  virtual ShowEpisodePtr ShowEpisode() const = 0;

  virtual int GetPlayCount() const = 0;

  virtual bool IncreasePlayCount() = 0;

  virtual const std::vector<MediaFilePtr>& Files() const = 0;

  /**
   *  @brief progress Returns the progress, in the [0;1] range
   */
  virtual float GetProgress() const = 0;

  virtual bool SetProgress(float progress) = 0;

  /**
   *  @brief rating The media rating, or -1 if unset.
   *
   *  It is up to the application to determine the values it wishes to use.
   *  No value is enforced, and any positive value (less or equal to INT32_MAX)
   *  will be accepted.
   */
  virtual int Rating() const = 0;

  virtual bool SetRating(int rating) = 0;

  virtual bool IsFavorite() const = 0;

  virtual bool SetFavorite(bool favorite) = 0;

  virtual bool AddLabel(LabelPtr label) = 0;

  virtual bool RemoveLabel(LabelPtr label) = 0;

  virtual std::vector<LabelPtr> GetLabels() = 0;

  virtual MoviePtr Movie() const = 0;

  virtual std::vector<VideoTrackPtr> VideoTracks() = 0;

  virtual std::vector<AudioTrackPtr> AudioTracks() = 0;

  /**
   * @brief  Returns the path of a thumbnail for this media
   * @return A path, relative to the thumbnailPath configured when initializing
   *         The media library
   */
  virtual const mstring& Thumbnail() = 0;

  virtual time_t InsertionDate() const = 0;

  virtual time_t ReleaseDate() const = 0;

  /**
  * @param metadataId
  *
  * @return
  */
  virtual bool AddMetadata(int64_t metadataId) = 0;

  /**
  * @param metadataId
  *
  * @return
  */
  virtual bool RemoveMetadata(int64_t metadataId) = 0;

  /**
  * @return
  */
  virtual std::vector<MetadataPtr> GetMetadata() const = 0;
};

} /* namespace mxp */

#endif /* MXP_IMEDIA_H */
