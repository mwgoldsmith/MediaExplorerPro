#pragma once

namespace msbuild {

template<typename T>
struct xml_traits<T> {
  typedef T value_type;

  enum { };
};
}