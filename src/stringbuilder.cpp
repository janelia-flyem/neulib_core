#include "core/stringbuilder.h"

#include <sstream>

neulib::StringBuilder::StringBuilder(const std::string &str)
{
  m_result = str;
}

neulib::StringBuilder::operator std::string()
{
  return m_result;
}

neulib::StringBuilder &neulib::StringBuilder::append(const std::string &str)
{
  m_result += str;

  return *this;
}

template<typename T,
         typename std::enable_if<std::is_integral<T>::value, T>::type>
neulib::StringBuilder& neulib::StringBuilder::append(T n, int pad)
{
  static_assert(
        std::is_integral<T>::value, "The first argument must be integral.");
  std::ostringstream stream;
  stream.fill('0');
  stream.width(pad);
  stream << n;

  m_result += stream.str();

  return *this;
}

template
neulib::StringBuilder& neulib::StringBuilder::append<int>(int num, int pad);

template
neulib::StringBuilder& neulib::StringBuilder::append<uint64_t>(uint64_t num, int pad);

template
neulib::StringBuilder& neulib::StringBuilder::append<int64_t>(int64_t num, int pad);
