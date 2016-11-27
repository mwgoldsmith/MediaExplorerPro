/*****************************************************************************
* Media Explorer
*****************************************************************************/

#pragma once

#include "stdafx.h"
#include <exception>

namespace mxp {

namespace sqlite {
namespace errors {

class ConstraintViolation : public std::exception {
public:
  ConstraintViolation(const std::string& req, const std::string& err) {
    m_reason = std::string("Request <") + req + "> aborted due to "
        "constraint violation (" + err + ")";
  }

  virtual const char* what() const noexcept override {
    return m_reason.c_str();
  }
private:
  std::string m_reason;
};

class ColumnOutOfRange : public std::exception {
public:
  ColumnOutOfRange(unsigned int idx, unsigned int nbColumns) {
    m_reason = "Attempting to extract column at index " + std::to_string(idx) +
        " from a request with " + std::to_string(nbColumns) + " columns";
  }

  virtual const char* what() const noexcept override {
    return m_reason.c_str();
  }

private:
  std::string m_reason;
};

} // namespace errors
} // namespace sqlite

} // namespace mxp
