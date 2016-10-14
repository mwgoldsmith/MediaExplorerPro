/*****************************************************************************
* Media Explorer
*****************************************************************************/

#ifndef CONTAINER_H
#define CONTAINER_H

#include "database/DatabaseHelpers.h"
#include "mediaexplorer/IContainer.h"
#include "mediaexplorer/MediaType.h"
#include "mediaexplorer/Types.h"
#include "Types.h"

namespace mxp {

class Container;

namespace policy {
struct ContainerTable {
  static const mstring Name;
  static const mstring PrimaryKeyColumn;
  static int64_t Container::*const PrimaryKey;
};
}

class Container : public IContainer, public DatabaseHelpers<Container, policy::ContainerTable> {
public:
  Container(MediaExplorerPtr ml, sqlite::Row& row);

  Container(MediaExplorerPtr ml, const mstring& name, const mstring& longName, const mstring& extensions, const mstring& mimeType, MediaType type);

  virtual int64_t Id() const noexcept override;

  virtual const mstring& GetName() const noexcept override;

  virtual const mstring& GetLongName() const noexcept override;

  virtual const mstring& GetExtensions() const noexcept override;

  virtual const mstring& GetMimeType() const noexcept override;

  virtual MediaType GetType() const noexcept override;

  virtual bool IsSupported() const noexcept override;

  virtual bool SetSupported(bool value) noexcept override;

  /**
  * @brief
  *
  * @param ml         Pointer to the MediaExplorer instance.
  * @param name       A comma separated list of short names for the format.
  * @param longName   Descriptive name for the format, meant to be more human-readable than name.
  * @param extensions Comma-separated list of file extensions for the container.
  * @param mimeType   Comma-separated list of mime types.
  * @param type       The most applicable MediaType for the container.
  *
  * @return If successful, std::shared_ptr to the created object; otherwise nullptr.
  */
  static ContainerPtr Create(MediaExplorerPtr ml, const mstring& name, const mstring& longName, const mstring& extensions, const mstring& mimeType, MediaType type) noexcept;

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
  static std::vector<ContainerPtr> ListAll(MediaExplorerPtr ml, SortingCriteria sort, bool desc);

private:
  MediaExplorerPtr     m_ml;
  int64_t              m_id;
  mstring              m_extensions;
  mstring              m_name;
  mstring              m_longName;
  mstring              m_mimeType;
  MediaType            m_type;
  bool                 m_isSupported;
  time_t               m_creationDate;

  friend struct policy::ContainerTable;
};

} /* namespace mxp */

#endif /* CONTAINER_H */
