/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#include "stdafx.h"
#if HAVE_CONFIG_H
# include "config.h"
#endif

#include "Codec.h"
#include "Media.h"
#include "Metadata.h"
#include "Stream.h"
#include "database/SqliteTools.h"

using mxp::policy::StreamTable;
const mstring StreamTable::Name = MTEXT("Stream");
const mstring StreamTable::PrimaryKeyColumn = MTEXT("id_stream");
int64_t mxp::Stream::* const StreamTable::PrimaryKey = &mxp::Stream::m_id;

mxp::Stream::Stream(MediaExplorerPtr ml, sqlite::Row& row)
  : m_ml(ml) {
  row >> m_id
      >> m_codecId
      >> m_index
      >> m_mediaId
      >> m_type
      >> m_creationDate;
}

mxp::Stream::Stream(MediaExplorerPtr ml, CodecPtr codec, uint32_t index, MediaPtr media, MediaType type)
  : m_ml(ml)
  , m_id(0)
  , m_codecId(0)
  , m_index(index)
  , m_mediaId(0)
  , m_type(type)
  , m_creationDate(time(nullptr)) {
  m_media = media;
  m_codec = codec;
  if(codec != nullptr) {
    m_codecId = codec->Id();
  }
  if(media != nullptr) {
    m_mediaId = media->Id();
  }
}

int64_t mxp::Stream::Id() const {
  return m_id;
}

uint32_t mxp::Stream::GetIndex() const {
  return m_index;
}

mxp::CodecPtr mxp::Stream::GetCodec() const {
  auto lock = m_codec.Lock();
  if(m_codec.IsCached() == false) {
    m_codec = m_ml->Codec(m_codecId);
  }

  return m_codec.Get();
}

mxp::MediaType mxp::Stream::GetType() const {
  return m_type;
}

mxp::MediaPtr mxp::Stream::GetMedia() const {
  auto lock = m_media.Lock();
  if(m_media.IsCached() == false) {
    m_media = m_ml->GetMedia(m_mediaId);
  }

  return m_media.Get();
}

bool mxp::Stream::AddMetadata(int64_t metadataId) {
  static const mstring req = "INSERT INTO StreamMetadataRelation(stream_id, metadata_id) VALUES(?, ?)";
  return sqlite::Tools::ExecuteInsert(m_ml->GetConnection(), req, m_id, metadataId) != 0;
}

bool mxp::Stream::RemoveMetadata(int64_t metadataId) {
  static const mstring req = "DELETE FROM StreamMetadataRelation WHERE stream_id = ? AND metadata_id = ?";
  return sqlite::Tools::ExecuteDelete(m_ml->GetConnection(), req, m_id, metadataId);
}

const std::vector<mxp::MetadataPtr>& mxp::Stream::GetMetadata() const {
  static const auto req = "SELECT m.* FROM " + StreamTable::Name + " s "
    " INNER JOIN StreamMetadataRelation smr ON smr.stream_id=s." + StreamTable::PrimaryKeyColumn +
    " INNER JOIN " + policy::MetadataTable::Name + " m ON smr.metadata_id=m." + policy::MetadataTable::PrimaryKeyColumn +
    " WHERE s." + StreamTable::PrimaryKeyColumn + " = ?";

  auto lock = m_metadata.Lock();
  if(m_metadata.IsCached() == false) {
    m_metadata = Metadata::FetchAll<IMetadata>(m_ml, req, m_id);
  }

  return m_metadata;
}

//********
// Static methods
mxp::StreamPtr mxp::Stream::Create(MediaExplorerPtr ml, CodecPtr codec, uint32_t index, MediaPtr media, MediaType type) noexcept {
  static const auto req = "INSERT INTO " + StreamTable::Name + "(codec_id, position, media_id, type, creation_date) VALUES(?, ?, ?, ?, ?)";
  std::shared_ptr<Stream> self;

  try {
    self = std::make_shared<Stream>(ml, codec, index, media, type);
    if(insert(ml, self, req, self->m_codecId, self->m_index, self->m_mediaId, self->m_type, self->m_creationDate) == false) {
      self = nullptr;
    }
  } catch(sqlite::errors::ConstraintViolation& ex) {
    LOG_ERROR(MTEXT("Failed to create Stream (SC-F-100): "), ex.what());
    self = nullptr;
  } catch(std::exception& ex) {
    LOG_ERROR(MTEXT("Failed to create Stream (SC-F-666): "), ex.what());
    self = nullptr;
  }

  return self;
}

bool mxp::Stream::CreateTable(DBConnection connection) noexcept {
  static const auto req = "CREATE TABLE IF NOT EXISTS " + StreamTable::Name + "(" + 
    StreamTable::PrimaryKeyColumn + " INTEGER PRIMARY KEY AUTOINCREMENT,"
    "codec_id UNSIGNED INTEGER NOT NULL,"
    "position UNSIGNED INTEGER NOT NULL,"
    "media_id UNSIGNED INTEGER NOT NULL,"
    "type INTEGER NOT NULL,"
    "creation_date UNSIGNED INTEGER NOT NULL,"
    "FOREIGN KEY(media_id) REFERENCES " + policy::MediaTable::Name + "(id_media) ON DELETE CASCADE,"
    "FOREIGN KEY(codec_id) REFERENCES " + policy::CodecTable::Name + "(id_codec) ON DELETE CASCADE"
    ")";
  static const auto relTableReq = "CREATE TABLE IF NOT EXISTS StreamMetadataRelation("
    "stream_id INTEGER,"
    "metadata_id INTEGER,"
    "PRIMARY KEY(stream_id, metadata_id),"
    "FOREIGN KEY(stream_id) REFERENCES " + StreamTable::Name + "(" + StreamTable::PrimaryKeyColumn + ") ON DELETE CASCADE,"
    "FOREIGN KEY(metadata_id) REFERENCES " + policy::MetadataTable::Name + "(" + policy::MetadataTable::PrimaryKeyColumn + ") ON DELETE CASCADE"
    ")";

  return Create(connection, req, StreamTable::Name) &&
      Create(connection, relTableReq, MTEXT("StreamMetadataRelation"));
}

bool mxp::Stream::Create(DBConnection connection, const mstring& sql, const mstring& name) noexcept {
  bool result;

  try {
    result = sqlite::Tools::ExecuteRequest(connection, sql);
  } catch(std::exception& ex) {
    LOG_ERROR(MTEXT("Failed to create table for "), name, MTEXT(": "), ex.what());
    result = false;
  }

  return result;
}

std::vector<mxp::StreamPtr> mxp::Stream::FindByMedia(MediaExplorerPtr ml, int64_t mediaId) {
  static const auto req = "SELECT * FROM " + StreamTable::Name + " WHERE media_id=?";

  return FetchAll<IStream>(ml, req, mediaId);;
}
