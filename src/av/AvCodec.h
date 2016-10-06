/*****************************************************************************
* Media Explorer
*****************************************************************************/

#ifndef AVCODEC_H
#define AVCODEC_H

#include <string>
#include "av/AvType.h"

namespace mxp {
namespace av {

class AvCodec {
public:
  AvCodec()
    : m_name({})
    , m_longName({})
    , m_type(AvType::None) {
  }

  /**
  *
  * @param name
  * @param longName
  * @param type
  *
  */
  AvCodec(const char* name, const char* longName, AvType type)
    : m_name(name)
    , m_longName(longName)
    , m_type(type) {
  }

  const std::string& GetName() const { return m_name; }
  const std::string& GetLongName() const { return m_longName; }
  AvType GetType() const { return m_type; }

private:
  const std::string m_name;
  const std::string m_longName;
  const AvType      m_type;
};

} /* namespace av */
} /* namespace mxp */

#endif /* AVCODEC_H */
