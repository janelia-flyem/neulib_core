#include <string>

namespace neulib
{

template <typename T>
void Assign(T *out, const T &v)
{
  if (out != NULL)
  {
    *out = v;
  }
}

std::string ToString(const void *p);

} // namespace neutu