/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#if HAVE_CONFIG_H
# include "config.h"
#endif

#include "Media.h"
#include "VideoTrack.h"

using mxp::policy::VideoTrackTable;
const std::string VideoTrackTable::Name = "VideoTrack";
const std::string VideoTrackTable::PrimaryKeyColumn = "id_track";
int64_t mxp::VideoTrack::* const VideoTrackTable::PrimaryKey = &mxp::VideoTrack::m_id;

mxp::VideoTrack::VideoTrack(MediaExplorerPtr, sqlite::Row& row) {
  row >> m_id
      >> m_codec
      >> m_width
      >> m_height
      >> m_fps
      >> m_mediaId
      >> m_language
      >> m_description;
}

mxp::VideoTrack::VideoTrack(MediaExplorerPtr, const std::string& codec, unsigned int width, unsigned int height,
                       float fps, int64_t mediaId, const std::string& language, const std::string& description)
  : m_id(0)
  , m_codec(codec)
  , m_width(width)
  , m_height(height)
  , m_fps(fps)
  , m_mediaId(mediaId)
  , m_language(language)
  , m_description(description) {}

int64_t mxp::VideoTrack::Id() const {
  return m_id;
}

const std::string& mxp::VideoTrack::Codec() const {
  return m_codec;
}

unsigned int mxp::VideoTrack::Width() const {
  return m_width;
}

unsigned int mxp::VideoTrack::Height() const {
  return m_height;
}

float mxp::VideoTrack::Fps() const {
  return m_fps;
}

const std::string& mxp::VideoTrack::Language() const {
  return m_language;
}

const std::string& mxp::VideoTrack::Description() const {
  return m_description;
}

std::shared_ptr<mxp::VideoTrack> mxp::VideoTrack::Create(MediaExplorerPtr ml, const std::string& codec, unsigned int width,
                                               unsigned int height, float fps, int64_t mediaId,
                                               const std::string& language, const std::string& description) {
  static const auto req = "INSERT INTO " + VideoTrackTable::Name
      + "(codec, width, height, fps, media_id, language, description) VALUES(?, ?, ?, ?, ?, ?, ?)";
  auto track = std::make_shared<VideoTrack>(ml, codec, width, height, fps, mediaId, language, description);
  if (insert(ml, track, req, codec, width, height, fps, mediaId, language, description) == false)
    return nullptr;
  return track;
}

bool mxp::VideoTrack::CreateTable(DBConnection dbConnection) {
  static const auto req = "CREATE TABLE IF NOT EXISTS " + VideoTrackTable::Name
      + "(" +
      VideoTrackTable::PrimaryKeyColumn + " INTEGER PRIMARY KEY AUTOINCREMENT,"
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
  return sqlite::Tools::ExecuteRequest(dbConnection, req);
}
