#pragma once

#include <QString>

template<typename T>
QString StdToQString(const T &);

template<>
inline QString StdToQString(const std::string &s) {
  return QString::fromStdString(s);
}

template<>
inline QString StdToQString(const std::wstring &s) {
  return QString::fromStdWString(s);
}

inline std::string QStringToStd(const QString& s) {
  return s.toStdString();
}
