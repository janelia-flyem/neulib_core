#include "core/utilities.h"

#include <sstream>

std::string neulib::ToString(const void *p)
{
  std::ostringstream stream;
  stream << p;
  return stream.str();
}