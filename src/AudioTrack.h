/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#ifndef AUDIOTRACK_H
#define AUDIOTRACK_H

#include "mediaexplorer/IAudioTrack.h"
#include "mediaexplorer/IMediaExplorer.h"
#include "database/DatabaseHelpers.h"

namespace mxp {

class AudioTrack;

namespace policy {
struct AudioTrackTable {
  static const std::string Name;
  static const std::string PrimaryKeyColumn;
  static int64_t AudioTrack::* const PrimaryKey;
};
}

class AudioTrack : public IAudioTrack, public DatabaseHelpers<AudioTrack, policy::AudioTrackTable> {
public:
  AudioTrack(MediaExplorerPtr ml, sqlite::Row& row);
  AudioTrack(MediaExplorerPtr ml, const std::string& codec, unsigned int bitrate,
             unsigned int sampleRate, unsigned int numChannels, const std::string& language,
             const std::string& desc, int64_t mediaId);

  virtual int64_t Id() const override;
  virtual const std::string& Codec() const override;
  virtual unsigned int Bitrate() const override;
  virtual unsigned int SampleRate() const override;
  virtual unsigned int NumChannels() const override;
  virtual const std::string& Language() const override;
  virtual const std::string& Description() const override;

  static bool CreateTable(DBConnection dbConnection);
  static std::shared_ptr<AudioTrack> Create(MediaExplorerPtr ml, const std::string& codec, unsigned int bitrate, unsigned int sampleRate, unsigned int numChannels, const std::string& language, const std::string& desc, int64_t mediaId);

private:
  int64_t      m_id;
  std::string  m_codec;
  unsigned int m_bitrate;
  unsigned int m_sampleRate;
  unsigned int m_nbChannels;
  std::string  m_language;
  std::string  m_description;
  int64_t      m_mediaId;

private:
  friend struct policy::AudioTrackTable;
};

}

#endif // AUDIOTRACK_H

