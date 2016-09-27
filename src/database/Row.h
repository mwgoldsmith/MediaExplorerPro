/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#ifndef ROW_H
#define ROW_H

namespace mxp {
namespace sqlite {

class Row {
public:
  explicit Row(sqlite3_stmt* stmt)
    : m_stmt(stmt)
    , m_idx(0)
    , m_nbColumns(sqlite3_column_count(stmt)) {
  }

  constexpr Row()
    : m_stmt(nullptr)
    , m_idx(0)
    , m_nbColumns(0) {
  }

  /**
   * @brief operator >> Extracts the next column from this result row.
   */
  template <typename T>
  Row& operator>>(T& t) {
    if(m_idx + 1 > m_nbColumns) {
      throw errors::ColumnOutOfRange(m_idx, m_nbColumns);
    }

#pragma warning( suppress : 4244 4800)
    t = sqlite::Traits<T>::Load(m_stmt, m_idx);
    m_idx++;
    return *this;
  }

  /**
   * @brief Returns the value in column idx, but doesn't advance to the next column
   */
  template <typename T>
  T load(unsigned int idx) const {
    if (m_idx + 1 > m_nbColumns)
      throw errors::ColumnOutOfRange(m_idx, m_nbColumns);
    return sqlite::Traits<T>::Load(m_stmt, idx);
  }

  bool operator==(std::nullptr_t) const {
    return m_stmt == nullptr;
  }

  bool operator!=(std::nullptr_t) const {
    return m_stmt != nullptr;
  }

private:
  sqlite3_stmt* m_stmt;
  unsigned int m_idx;
  unsigned int m_nbColumns;
};

} /* namespace sqlite */
} /* namespace mxp */

#endif /* ROW_H */