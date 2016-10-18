/*****************************************************************************
* Media Explorer
*****************************************************************************/

#ifndef CODEC_H
#define CODEC_H

#include "database/DatabaseHelpers.h"
#include "mediaexplorer/ICodec.h"
#include "mediaexplorer/MediaType.h"
#include "mediaexplorer/Common.h"
#include "Types.h"

namespace mxp {

class Codec;

namespace policy {
struct CodecTable {
  static const mstring Name;
  static const mstring PrimaryKeyColumn;
  static int64_t Codec::*const PrimaryKey;
};
}

class Codec : public ICodec, public DatabaseHelpers<Codec, policy::CodecTable> {
public:
  Codec(MediaExplorerPtr ml, sqlite::Row& row);

  Codec(MediaExplorerPtr ml, const mstring& name, const mstring& longName, MediaType type);

  virtual int64_t Id() const noexcept override;

  virtual const mstring& GetName() const noexcept override;

  virtual const mstring& GetLongName() const noexcept override;

  virtual MediaType GetType() const noexcept override;

  virtual bool IsSupported() const noexcept override;

  virtual bool SetSupported(bool value) noexcept override;

  /**
   * @brief
   *
   * @param ml         Pointer to the MediaExplorer instance.
   * @param name       A comma separated list of short names for the format.
   * @param longName   Descriptive name for the format, meant to be more human-readable than name.
   * @param type       The type of media the codec is for.
   *
   * @return If successful, std::shared_ptr to the created object; otherwise nullptr.
   */
  static CodecPtr Create(MediaExplorerPtr ml, const mstring& name, const mstring& longName, MediaType type) noexcept;

  /**
   * @brief
   *
   * @param connection
   *
   * @return If successful, true; otherwise false.
   */
  static bool CreateTable(DBConnection connection) noexcept;

  /**
   * @brief
   *
   * @param ml         Pointer to the MediaExplorer instance.
   * @param sort
   * @param desc
   *
   * @return 
   */
  static std::vector<CodecPtr> ListAll(MediaExplorerPtr ml, SortingCriteria sort, bool desc);

private:
  MediaExplorerPtr m_ml;
  int64_t          m_id;
  mstring          m_name;
  mstring          m_longName;
  MediaType        m_type;
  bool             m_isSupported;
  time_t           m_creationDate;

  friend struct policy::CodecTable;
};

} /* namespace mxp */


#endif /* CODEC_H */
