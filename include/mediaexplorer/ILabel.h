/*****************************************************************************
* Media Explorer
*****************************************************************************/

#ifndef MXP_ILABEL_H
#define MXP_ILABEL_H


#include "mediaexplorer/Common.h"

namespace mxp {

class ILabel {
public:
  virtual ~ILabel() {}

  /**
   * @return Identifier of the record for the label
   */
  virtual int64_t Id() const = 0;

  /**
   * @return Name of the label
   */
  virtual const std::string& Name() const = 0;

  /**
   * @return Vector of pointers to all of the media records with this label
   */
  virtual std::vector<MediaPtr> Files() = 0;
};

} /* namespace mxp */

#endif /* MXP_ILABEL_H */
