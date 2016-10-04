/*****************************************************************************
* Media Explorer
*****************************************************************************/

#ifndef MXP_HISTORYENTRY_H
#define MXP_HISTORYENTRY_H

#include <string>

namespace mxp {

class IHistoryEntry {
public:
  virtual ~IHistoryEntry() = default;

  virtual const std::string& mrl() const = 0;

  virtual unsigned int InsertionDate() const = 0;

  virtual bool IsFavorite() const = 0;

  virtual bool SetFavorite(bool isFavorite) = 0;
};

} /* namespace mxp */

#endif /* MXP_HISTORYENTRY_H */
