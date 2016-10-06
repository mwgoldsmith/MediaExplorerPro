/*****************************************************************************
* Media Explorer
*****************************************************************************/

#ifndef AVCONTAINER_H
#define AVCONTAINER_H

#include <string>

namespace mxp {
namespace av {

class AvContainer {
public:
  AvContainer() 
    : m_name({})
    , m_longName({})
    , m_extensions({})
    , m_mimeType({}) {
  }

  /**
   *
   * @param name
   * @param longName
   * @param extensions
   * @param mimeType
   *
   */
  AvContainer(const char* name, const char* longName, const char* extensions, const char* mimeType)
    : m_name(name)
    , m_longName(longName)
    , m_extensions(extensions)
    , m_mimeType(mimeType) {
  }

  const std::string& GetName() const { return m_name; }
  const std::string& GetLongName() const { return m_longName; }
  const std::string& GetExtensions() const { return m_extensions; }
  const std::string& GetMimeType() const { return m_mimeType; }

private:
  const std::string m_name;
  const std::string m_longName;
  const std::string m_extensions;
  const std::string m_mimeType;
};

} /* namespace av */
} /* namespace mxp */

#endif /* AVCONTAINER_H */
