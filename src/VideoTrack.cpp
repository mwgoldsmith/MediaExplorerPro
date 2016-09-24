/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#if HAVE_CONFIG_H
# include "config.h"
#endif

#include "Media.h"
#include "VideoTrack.h"

namespace mxp {

const std::string policy::VideoTrackTable::Name = "VideoTrack";
const std::string policy::VideoTrackTable::PrimaryKeyColumn = "id_track";
int64_t VideoTrack::* const policy::VideoTrackTable::PrimaryKey = &VideoTrack::m_id;

VideoTrack::VideoTrack(MediaExplorerPtr, sqlite::Row& row) {
  row >> m_id
      >> m_codec
      >> m_width
      >> m_height
      >> m_fps
      >> m_mediaId
      >> m_language
      >> m_description;
}

VideoTrack::VideoTrack(MediaExplorerPtr, const std::string& codec, unsigned int width, unsigned int height,
                       float fps, int64_t mediaId, const std::string& language, const std::string& description)
  : m_id(0)
    , m_codec(codec)
    , m_width(width)
    , m_height(height)
    , m_fps(fps)
    , m_mediaId(mediaId)
    , m_language(language)
    , m_description(description) {}

int64_t VideoTrack::id() const {
  return m_id;
}

const std::string& VideoTrack::codec() const {
  return m_codec;
}

unsigned int VideoTrack::width() const {
  return m_width;
}

unsigned int VideoTrack::height() const {
  return m_height;
}

float VideoTrack::fps() const {
  return m_fps;
}

const std::string& VideoTrack::language() const {
  return m_language;
}

const std::string& VideoTrack::description() const {
  return m_description;
}

std::shared_ptr<VideoTrack> VideoTrack::create(MediaExplorerPtr ml, const std::string& codec, unsigned int width,
                                               unsigned int height, float fps, int64_t mediaId,
                                               const std::string& language, const std::string& description) {
  static const std::string req = "INSERT INTO " + policy::VideoTrackTable::Name
      + "(codec, width, height, fps, media_id, language, description) VALUES(?, ?, ?, ?, ?, ?, ?)";
  auto track = std::make_shared<VideoTrack>(ml, codec, width, height, fps, mediaId, language, description);
  if (insert(ml, track, req, codec, width, height, fps, mediaId, language, description) == false)
    return nullptr;
  return track;
}

bool VideoTrack::createTable(DBConnection dbConnection) {
  static const std::string req = "CREATE TABLE IF NOT EXISTS " + policy::VideoTrackTable::Name
      + "(" +
      policy::VideoTrackTable::PrimaryKeyColumn + " INTEGER PRIMARY KEY AUTOINCREMENT,"
      "codec TEXT,"
      "width UNSIGNED INTEGER,"
      "height UNSIGNED INTEGER,"
      "fps FLOAT,"
      "media_id UNSIGNED INT,"
      "language TEXT,"
      "description TEXT,"
      "FOREIGN KEY ( media_id ) REFERENCES " + policy::MediaTable::Name +
      "(id_media) ON DELETE CASCADE"
      ")";
  return sqlite::Tools::executeRequest(dbConnection, req);
}

}
