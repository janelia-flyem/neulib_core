#ifndef NEULIB_CORE_STRINGBUILDER_H
#define NEULIB_CORE_STRINGBUILDER_H

#include <type_traits>
#include <string>
#include <sstream>
#include <cstdint>
#include <cstdio>

#include "utilities.h"

namespace neulib {

class StringBuilder
{
public:
  StringBuilder(const std::string &str);

  operator std::string ();

  StringBuilder& append(const std::string &str);

  StringBuilder& replace(const std::string &from, const std::string &to);

  template<typename T,
           typename std::enable_if<std::is_integral<T>::value, T>::type = 0>
  StringBuilder& append(const T& v) {
    std::ostringstream stream;
    stream << v;
    m_result += stream.str();
    return *this;
  }

  template<typename T,
           typename std::enable_if<std::is_integral<T>::value, T>::type = 0>
  StringBuilder& append(T n, int pad);

  StringBuilder& arg(const std::string &str);
  StringBuilder& arg(const char *str);

  template<typename T>
  StringBuilder& arg(const T &v);

  StringBuilder& argend();

private:
  std::string m_result;
  static const char* PLACE_MARKER;
  static const char* PLACE_MARKER_ESCAPE;
};

extern template
StringBuilder& StringBuilder::append<int>(int num, int pad);

extern template
StringBuilder& StringBuilder::append<int64_t>(int64_t num, int pad);

extern template
StringBuilder& StringBuilder::append<uint64_t>(uint64_t num, int pad);

template<typename T>
StringBuilder& StringBuilder::arg(const T &v)
{
  std::ostringstream stream;
  stream << v;

  return arg(stream.str());
}

}

#endif