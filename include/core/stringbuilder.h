#ifndef NEULIB_CORE_STRINGBUILDER_H
#define NEULIB_CORE_STRINGBUILDER_H

#include <type_traits>
#include <string>
#include <cstdint>

namespace neulib {

class StringBuilder
{
public:
  StringBuilder(const std::string &str);

  operator std::string ();

  StringBuilder& append(const std::string &str);

  template<typename T,
           typename std::enable_if<std::is_integral<T>::value, T>::type = 0>
  StringBuilder& append(
      const T& v) {
    m_result += std::to_string(v);
    return *this;
  }

  template<typename T,
           typename std::enable_if<std::is_integral<T>::value, T>::type = 0>
  StringBuilder& append(T n, int pad);

private:
  std::string m_result;
};

extern template
StringBuilder& StringBuilder::append<int>(int num, int pad);

extern template
StringBuilder& StringBuilder::append<int64_t>(int64_t num, int pad);

extern template
StringBuilder& StringBuilder::append<uint64_t>(uint64_t num, int pad);

}

#endif