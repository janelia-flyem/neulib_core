#include "core/stringbuilder.h"

#include <sstream>

const char* neulib::StringBuilder::PLACE_MARKER = "[$]";
const char* neulib::StringBuilder::PLACE_MARKER_ESCAPE = "[\\$]";

neulib::StringBuilder::StringBuilder(const std::string &str)
{
  m_result = str;
}

neulib::StringBuilder::operator std::string()
{
  return m_result;
}

neulib::StringBuilder& neulib::StringBuilder::replace(
  const std::string &from, const std::string &to)
{
  if (!from.empty()) {
    std::size_t pos = 0;
    while ((pos = m_result.find(from, pos)) != std::string::npos) {
      m_result.replace(pos, from.length(), to);
      pos += to.length();
    }
  }

  return (*this);
}

neulib::StringBuilder& neulib::StringBuilder::append(const std::string &str)
{
  m_result += str;

  return *this;
}
neulib::StringBuilder& neulib::StringBuilder::arg(const char *str)
{
  return arg(std::string(str));
}

neulib::StringBuilder& neulib::StringBuilder::arg(const std::string &str)
{
  std::size_t pos = m_result.find(PLACE_MARKER);
  if (pos != std::string::npos) {
    m_result.replace(pos, strlen(PLACE_MARKER), str);
  }

  return *this;
}

neulib::StringBuilder& neulib::StringBuilder::argend()
{
  replace(PLACE_MARKER_ESCAPE, PLACE_MARKER);

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
neulib::StringBuilder& neulib::StringBuilder::append<uint64_t>(
  uint64_t num, int pad);

template
neulib::StringBuilder& neulib::StringBuilder::append<int64_t>(
  int64_t num, int pad);
