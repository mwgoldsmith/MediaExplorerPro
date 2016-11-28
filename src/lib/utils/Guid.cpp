/*****************************************************************************
* Media Explorer
*****************************************************************************/

#include "stdafx.h"
#if HAVE_CONFIG_H
# include "config.h"
#endif

#include "utils/Guid.h"
#include <objbase.h>

mxp::utils::Guid::Guid()
  : m_bytes{ 0 } {
}

mxp::utils::Guid::Guid(const std::array<unsigned char, 16>& bytes) {
  m_bytes = bytes;
}

mxp::utils::Guid::Guid(const unsigned char *bytes) {
  for (auto i = 0; i < 16; i++)
    m_bytes[i] = *bytes++;
}

static constexpr unsigned char hexCharToByte(unsigned char ch) {
  return (ch > 47 && ch < 58)
    ? (ch - 48)
    : (ch > 96 && ch < 103)
    ? (ch - 87)
    : (ch > 64 && ch < 71)
    ? (ch - 55)
    : 0;
}

mxp::utils::Guid::Guid(const std::string &str) {
  auto it = str.cbegin();
  for (auto i = 0; i < 16; i++) {
    char a = '0', b = '0';
    if (it != str.cend())
      a = *it++;
    if (a == '-')
      continue;
    if (it != str.cend())
      b = *it++;

    m_bytes[i] = hexCharToByte(a) * 16 + hexCharToByte(b);
  }
}

  mxp::utils::Guid::Guid(const Guid &other) {
  m_bytes = other.m_bytes;
}

mxp::utils::Guid &mxp::utils::Guid::operator=(const Guid &other) {
  m_bytes = other.m_bytes;
  return *this;
}

bool mxp::utils::Guid::operator==(const Guid &other) const {
  return m_bytes == other.m_bytes;
}

bool mxp::utils::Guid::operator!=(const Guid &other) const {
  return !((*this) == other);
}

static const char s_hex[] = "0123456789ABCDEF";
static const char s_num[] = "0123456789";
static const int s_guid_bytes = 16;
static const int s_guid_chars = (s_guid_bytes * 2) + 4;

std::string mxp::utils::Guid::ToString() const {
  char buffer[s_guid_chars + 1];
  auto cur = &buffer[0];

  for (auto i = 0; i < 16; ++i) {
    auto value = m_bytes[i];

    for (int n = (sizeof(unsigned char) * 2) - 1; n >= 0; n--) {
      auto num = value % 16;
      *(cur + n) = s_hex[num];
      value /= 16;
    }

    cur += sizeof(unsigned char) * 2;
    if (i == 3 || i == 5 || i == 7 || i == 9)
      *cur++ = '-';
  }

  *cur = 0;

  return std::string(buffer);
}

mxp::utils::Guid mxp::utils::Guid::Create() {
#ifdef HAVE_WIN32
  GUID newId;
  CoCreateGuid(&newId);

  const unsigned char bytes[16] = {
    static_cast<unsigned char>((newId.Data1 >> 24) & 0xFF),
    static_cast<unsigned char>((newId.Data1 >> 16) & 0xFF),
    static_cast<unsigned char>((newId.Data1 >> 8)  & 0xFF),
    static_cast<unsigned char>((newId.Data1)       & 0xFF),
    static_cast<unsigned char>((newId.Data2 >> 8)  & 0xFF),
    static_cast<unsigned char>((newId.Data2)       & 0xFF),
    static_cast<unsigned char>((newId.Data3 >> 8)  & 0xFF),
    static_cast<unsigned char>((newId.Data3)       & 0xFF),
    newId.Data4[0],
    newId.Data4[1],
    newId.Data4[2],
    newId.Data4[3],
    newId.Data4[4],
    newId.Data4[5],
    newId.Data4[6],
    newId.Data4[7]
  };

  return bytes;
#else
  return{};
#endif
}