/*****************************************************************************
* Media Explorer
*****************************************************************************/

#ifndef MXP_ILIBRARYFOLDER_H
#define MXP_ILIBRARYFOLDER_H

#include "mediaexplorer/Common.h"

namespace mxp {

class ILibraryFolder {
public:
  virtual ~ILibraryFolder() {}

  /**
   * @return Identifier of the record for the library folder
   */
  virtual int64_t Id() const = 0;

  /**
   * @return Name of the library folder
   */
  virtual const std::string& GetName() const = 0;

  /**
   * @return
   */
  virtual bool SetName(const std::string& name) = 0;

  /**
   * @return True if the folder is a virtual folder; false if useer defined.
   */
  virtual bool IsVirtual() const = 0;

  /**
   * @return
   */
  virtual bool IsHidden() const = 0;

  /**
   * @return
   */
  virtual bool SetHidden(bool value) = 0;

  /**
   * @return
   */
  virtual unsigned int GetPosition() const = 0;

  /**
   * @return
   */
  virtual bool SetPosition(unsigned int value) = 0;

  /**
   * @return
   */
  virtual bool AddMedia(int64_t mediaId) = 0;

  /**
   * @return
   */
  virtual bool SetMediaPosition(int64_t mediaId, unsigned int position) = 0;

  /**
   * @return
   */
  virtual bool RemoveMedia(int64_t mediaId) = 0;

  /**
   * @return Pointer to the parent library folder
   */
  virtual const LibraryFolderPtr& GetParent() const = 0;

  /**
   * @return The ID of the parent library folder
   */
  virtual int64_t GetParentId() const = 0;

  /**
   * @return
   */
  virtual bool SetParent(int64_t libraryFolderId) = 0;

  /**
   * @return
   */
  virtual std::vector<LibraryFolderPtr> GetChildren() const = 0;
  
  /**
   * @return Vector of pointers to all of the media records in this library folder
   */
  virtual std::vector<MediaPtr> GetMedia() const = 0;
};

} /* namespace mxp */

#endif /* MXP_ILIBRARYFOLDER_H */
