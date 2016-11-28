#ifndef GUID_H
#define GUID_H

#include <mediaexplorer/Common.h>
#include <array>

namespace mxp {
namespace utils {

class Guid {

public:
  Guid();
  Guid(const std::string& str);
  Guid(const std::array<unsigned char, 16> &bytes);
  Guid(const unsigned char* bytes);

  Guid(const Guid &other);

  Guid& operator=(const Guid& other);

  bool operator==(const Guid& other) const;
  bool operator!=(const Guid& other) const;

  std::string ToString() const;

  static Guid Create();

private:
  std::array<unsigned char, 16> m_bytes;
};

} /* namespace utils */
} /* namespace mxp */

#endif /* GUID_H */