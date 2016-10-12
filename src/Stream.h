/*****************************************************************************
* Media Explorer
*****************************************************************************/

#ifndef STREAM_H
#define STREAM_H

#include "database/DatabaseHelpers.h"
#include "mediaexplorer/IStream.h"
#include "mediaexplorer/MediaType.h"
#include "mediaexplorer/Types.h"
#include "Types.h"

namespace mxp {

class Stream;

namespace policy {
struct StreamTable {
  static const mstring Name;
  static const mstring PrimaryKeyColumn;
  static int64_t Stream::*const PrimaryKey;
};
}

class Stream : public IStream, public DatabaseHelpers<Stream, policy::StreamTable> {
public:
  Stream(MediaExplorerPtr ml, sqlite::Row& row);

  Stream(MediaExplorerPtr ml, CodecPtr codec, uint32_t index, MediaPtr media, MediaType type);

  virtual int64_t Id() const override;

  virtual CodecPtr GetCodec() const override;

  virtual uint32_t GetIndex() const override;

  virtual MediaPtr GetMedia() const override;

  virtual const std::vector<MetadataPtr>& GetMetadata() const override;

  virtual MediaType GetType() const override;

  virtual bool AddMetadata(int64_t metadataId) override;

  virtual bool RemoveMetadata(int64_t metadataId) override;

  static StreamPtr Create(MediaExplorerPtr ml, CodecPtr codec, uint32_t index, MediaPtr media, MediaType type) noexcept;

  static bool CreateTable(DBConnection dbConnection) noexcept;

private:
  static bool Create(DBConnection connection, const mstring& sql, const mstring& name) noexcept;

  MediaExplorerPtr         m_ml;
  int64_t                  m_id;
  int64_t                  m_codecId;
  uint32_t                 m_index;
  int64_t                  m_mediaId;
  MediaType                m_type;
  time_t                   m_creationDate;
  mutable Cache<MediaPtr>                 m_media;
  mutable Cache<CodecPtr>                 m_codec;
  mutable Cache<std::vector<MetadataPtr>> m_metadata;

  friend struct policy::StreamTable;
};

} /* namespace mxp */


#endif /* STREAM_H */
