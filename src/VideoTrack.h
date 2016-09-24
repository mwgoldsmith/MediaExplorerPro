/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#ifndef VIDEOTRACK_H
#define VIDEOTRACK_H

#if HAVE_CONFIG_H
# include "config.h"
#endif

#include "database/DatabaseHelpers.h"
#include "mediaexplorer/IVideoTrack.h"

namespace mxp {

class VideoTrack;

namespace policy {
struct VideoTrackTable {
  static const std::string Name;
  static const std::string PrimaryKeyColumn;
  static int64_t VideoTrack::* const PrimaryKey;
};
}

class VideoTrack : public IVideoTrack, public DatabaseHelpers<VideoTrack, policy::VideoTrackTable> {
public:
  VideoTrack(MediaExplorerPtr, sqlite::Row& row);
  VideoTrack(MediaExplorerPtr, const std::string& codec,
             unsigned int width, unsigned int height, float fps, int64_t mediaId,
             const std::string& language, const std::string& description);

  virtual int64_t id() const override;
  virtual const std::string& codec() const override;
  virtual unsigned int width() const override;
  virtual unsigned int height() const override;
  virtual float fps() const override;
  virtual const std::string& language() const override;
  virtual const std::string& description() const override;

  static bool createTable(DBConnection dbConnection);
  static std::shared_ptr<VideoTrack> create(MediaExplorerPtr ml, const std::string& codec,
                                            unsigned int width, unsigned int height, float fps, int64_t mediaId,
                                            const std::string& language, const std::string& description);

private:
  int64_t m_id;
  std::string m_codec;
  unsigned int m_width;
  unsigned int m_height;
  float m_fps;
  int64_t m_mediaId;
  std::string m_language;
  std::string m_description;

private:
  friend struct policy::VideoTrackTable;
};

}

#endif // VIDEOTRACK_H

