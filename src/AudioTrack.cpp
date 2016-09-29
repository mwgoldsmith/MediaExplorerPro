/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#if HAVE_CONFIG_H
# include "config.h"
#endif

#include "AudioTrack.h"
#include "Media.h"

using mxp::policy::AudioTrackTable;
const std::string AudioTrackTable::Name = "AudioTrack";
const std::string AudioTrackTable::PrimaryKeyColumn = "id_track";
int64_t mxp::AudioTrack::* const AudioTrackTable::PrimaryKey = &mxp::AudioTrack::m_id;

mxp::AudioTrack::AudioTrack(MediaExplorerPtr, sqlite::Row& row) {
  row >> m_id
      >> m_codec
      >> m_bitrate
      >> m_sampleRate
      >> m_nbChannels
      >> m_language
      >> m_description
      >> m_mediaId;
}

mxp::AudioTrack::AudioTrack(MediaExplorerPtr, const std::string& codec, unsigned int bitrate, unsigned int sampleRate,
                       unsigned int nbChannels, const std::string& language, const std::string& desc,
                       int64_t mediaId)
  : m_id(0)
  , m_codec(codec)
  , m_bitrate(bitrate)
  , m_sampleRate(sampleRate)
  , m_nbChannels(nbChannels)
  , m_language(language)
  , m_description(desc)
  , m_mediaId(mediaId) {}

int64_t mxp::AudioTrack::id() const {
  return m_id;
}

const std::string& mxp::AudioTrack::codec() const {
  return m_codec;
}

unsigned int mxp::AudioTrack::bitrate() const {
  return m_bitrate;
}

unsigned int mxp::AudioTrack::SampleRate() const {
  return m_sampleRate;
}

unsigned int mxp::AudioTrack::nbChannels() const {
  return m_nbChannels;
}

const std::string& mxp::AudioTrack::language() const {
  return m_language;
}

const std::string& mxp::AudioTrack::description() const {
  return m_description;
}

bool mxp::AudioTrack::CreateTable(DBConnection dbConnection) {
  //FIXME: Index on media_id ? Unless it's already implied by the foreign key
  static const auto req = "CREATE TABLE IF NOT EXISTS " + AudioTrackTable::Name
      + "(" +
      AudioTrackTable::PrimaryKeyColumn + " INTEGER PRIMARY KEY AUTOINCREMENT,"
      "codec TEXT,"
      "bitrate UNSIGNED INTEGER,"
      "samplerate UNSIGNED INTEGER,"
      "nb_channels UNSIGNED INTEGER,"
      "language TEXT,"
      "description TEXT,"
      "media_id UNSIGNED INT,"
      "FOREIGN KEY ( media_id ) REFERENCES " + policy::MediaTable::Name
      + "( id_media ) ON DELETE CASCADE"
      ")";
  return sqlite::Tools::ExecuteRequest(dbConnection, req);
}

std::shared_ptr<mxp::AudioTrack> mxp::AudioTrack::Create(MediaExplorerPtr ml, const std::string& codec,
                                               unsigned int bitrate, unsigned int sampleRate, unsigned int nbChannels,
                                               const std::string& language, const std::string& desc, int64_t mediaId) {
  static const auto req = "INSERT INTO " + AudioTrackTable::Name
      + "(codec, bitrate, samplerate, nb_channels, language, description, media_id) VALUES(?, ?, ?, ?, ?, ?, ?)";
  auto track = std::make_shared<AudioTrack>(ml, codec, bitrate, sampleRate, nbChannels, language, desc, mediaId);
  if (insert(ml, track, req, codec, bitrate, sampleRate, nbChannels, language, desc, mediaId) == false)
    return nullptr;
  return track;
}
