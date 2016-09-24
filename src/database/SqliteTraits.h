/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#pragma once

#include <sqlite3.h>

namespace mxp {
namespace sqlite {

struct ForeignKey {
  constexpr explicit ForeignKey(unsigned int v) : value(v) {}
  unsigned int value;
};

template <typename ToCheck, typename T>
using IsSameDecay = std::is_same<typename std::decay<ToCheck>::type, T>;

template <typename T, typename Enable = void>
struct Traits;

template <typename T>
struct Traits<T, typename std::enable_if<
    std::is_integral<typename std::decay<T>::type>::value
    && ! IsSameDecay<T, int64_t>::value
  >::type> {
  static constexpr
  int (*Bind)(sqlite3_stmt *, int, int) = &sqlite3_bind_int;

  static constexpr
  int (*Load)(sqlite3_stmt *, int) = &sqlite3_column_int;
};

template <typename T>
struct Traits<T, typename std::enable_if<IsSameDecay<T, ForeignKey>::value>::type> {
  static int Bind(sqlite3_stmt *stmt, int pos, ForeignKey fk) {
    if (fk.value != 0)
      return Traits<unsigned int>::Bind(stmt, pos, fk.value);
    return sqlite3_bind_null(stmt, pos);
  }
};

template <typename T>
struct Traits<T, typename std::enable_if<IsSameDecay<T, std::string>::value>::type> {
  static int Bind(sqlite3_stmt* stmt, int pos, const std::string& value) {
    return sqlite3_bind_text(stmt, pos, value.c_str(), -1, SQLITE_STATIC);
  }

  static std::string Load(sqlite3_stmt* stmt, int pos) {
    auto tmp = (const char*)sqlite3_column_text(stmt, pos);
    if (tmp != nullptr)
      return std::string(tmp);
    return std::string();
  }
};

template <typename T>
struct Traits<T, typename std::enable_if<std::is_floating_point<
    typename std::decay<T>::type
  >::value>::type> {
  static constexpr int
  (*Bind)(sqlite3_stmt *, int, double) = &sqlite3_bind_double;

  static constexpr double
  (*Load)(sqlite3_stmt *, int) = &sqlite3_column_double;
};

template <>
struct Traits<std::nullptr_t> {
  static int Bind(sqlite3_stmt* stmt, int idx, std::nullptr_t) {
    return sqlite3_bind_null(stmt, idx);
  }
};

template <typename T>
struct Traits<T, typename std::enable_if<std::is_enum<
    typename std::decay<T>::type
  >::value>::type> {
  using type_t = typename std::underlying_type<typename std::decay<T>::type>::type;
  static int Bind(sqlite3_stmt* stmt, int pos, T value) {
    return sqlite3_bind_int(stmt, pos, static_cast<type_t>(value));
  }

  static T Load(sqlite3_stmt* stmt, int pos) {
    return static_cast<T>(sqlite3_column_int(stmt, pos));
  }
};

template <typename T>
struct Traits<T, typename std::enable_if<IsSameDecay<T, int64_t>::value>::type> {
  static constexpr int
  (*Bind)(sqlite3_stmt *, int, sqlite_int64) = &sqlite3_bind_int64;

  static constexpr sqlite_int64
  (*Load)(sqlite3_stmt *, int) = &sqlite3_column_int64;
};

} /* namespace sqlite */
} /* namespace mxp */
