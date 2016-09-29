/*****************************************************************************
* Media Explorer
*****************************************************************************/

#pragma once

#include <string>

namespace mxp {

class IHistoryEntry {
public:
  virtual ~IHistoryEntry() = default;
  virtual const std::string& mrl() const = 0;
  virtual unsigned int InsertionDate() const = 0;
  virtual bool isFavorite() const = 0;
  virtual bool SetFavorite(bool isFavorite) = 0;
};

}
