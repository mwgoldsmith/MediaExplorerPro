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
  virtual unsigned int insertionDate() const = 0;
  virtual bool isFavorite() const = 0;
  virtual bool setFavorite(bool isFavorite) = 0;
};

}
