/*****************************************************************************
* Media Explorer
*****************************************************************************/

#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

#include "compat/Mutex.h"
#include "SqliteTools.h"

namespace mxp {

template <typename IMPL, typename TABLEPOLICY>
class DatabaseHelpers {
  using Lock = std::lock_guard<compat::Mutex>;

public:
  template <typename... Args>
  static std::shared_ptr<IMPL> Fetch(MediaExplorerPtr ml, const std::string& req, Args&&... args) {
    return sqlite::Tools::FetchOne<IMPL>(ml, req, std::forward<Args>(args)...);
  }

  static std::shared_ptr<IMPL> Fetch(MediaExplorerPtr ml, int64_t pkValue) {
    static std::string req = "SELECT * FROM " + TABLEPOLICY::Name + " WHERE " + TABLEPOLICY::PrimaryKeyColumn + " = ?";
    return sqlite::Tools::FetchOne<IMPL>(ml, req, pkValue);
  }

  /*
    * Will fetch all elements from the database & cache them.
    */
  template <typename INTF = IMPL>
  static std::vector<std::shared_ptr<INTF>> FetchAll(MediaExplorerPtr ml) {
    static const std::string req = "SELECT * FROM " + TABLEPOLICY::Name;
    return sqlite::Tools::FetchAll<IMPL, INTF>(ml, req);
  }

  template <typename INTF, typename... Args>
  static std::vector<std::shared_ptr<INTF>> FetchAll(MediaExplorerPtr ml, const std::string &req, Args&&... args) {
    return sqlite::Tools::FetchAll<IMPL, INTF>(ml, req, std::forward<Args>(args)...);
  }

  static std::shared_ptr<IMPL> Load(MediaExplorerPtr ml, sqlite::Row& row) {
    Lock l{ Mutex };

    auto key = row.Load<int64_t>(0);
    auto it = Store.find(key);
    if (it != Store.end())
      return it->second;
    auto res = std::make_shared<IMPL>(ml, row);
    Store[key] = res;
    return res;
  }

  static bool destroy(MediaExplorerPtr ml, int64_t pkValue) {
    static const std::string req = "DELETE FROM " + TABLEPOLICY::Name + " WHERE " + TABLEPOLICY::PrimaryKeyColumn + " = ?";
    auto res = sqlite::Tools::ExecuteDelete(ml->GetConnection(), req, pkValue);
    if (res == true)
      RemoveFromCache(pkValue);
    return res;
  }

  static void RemoveFromCache(int64_t pkValue) {
    Lock l{ Mutex };
    auto it = Store.find(pkValue);
    if (it != end(Store))
      Store.erase(it);
  }

  static void clear() {
    Lock l{ Mutex };
    Store.clear();
  }

protected:
  /*
    * Create a new instance of the cache class.
    */
  template <typename... Args>
  static bool insert(MediaExplorerPtr ml, std::shared_ptr<IMPL> self, const std::string& req, Args&&... args) {
    int64_t pKey = sqlite::Tools::ExecuteInsert(ml->GetConnection(), req, std::forward<Args>(args)...);
    if (pKey == 0)
      return false;
    Lock l{ Mutex };
    (self.get())->*TABLEPOLICY::PrimaryKey = pKey;
    assert(Store.find(pKey) == end(Store));
    Store[pKey] = self;
    return true;
  }

private:
  static std::unordered_map<int64_t, std::shared_ptr<IMPL>> Store;
  static compat::Mutex Mutex;
};


template <typename IMPL, typename TABLEPOLICY>
std::unordered_map<int64_t, std::shared_ptr<IMPL>>
DatabaseHelpers<IMPL, TABLEPOLICY>::Store;

template <typename IMPL, typename TABLEPOLICY>
compat::Mutex DatabaseHelpers<IMPL, TABLEPOLICY>::Mutex;

}
