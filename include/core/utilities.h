#include <string>
#include <map>
#include <mutex>
#include <tuple>

namespace neulib
{

template <typename T>
void Assign(T *out, const T &v)
{
  if (out != NULL) {
    *out = v;
  }
}

std::string ToString(const void *p);

namespace {
std::mutex memoize_mutex;
}

template <typename T, typename... Args>
std::function<T(Args...)> Memoize(T (*fn)(Args...))
{
  std::map<std::tuple<Args...>, T> cache;
  return [fn, cache](Args... args) mutable -> T {
    std::lock_guard<std::mutex> guard(memoize_mutex);
    auto argTuple = std::make_tuple(args...);
    auto memoized = cache.find(argTuple);
    if (memoized == cache.end()) {
      return (cache[argTuple] = fn(args...));
    } else {
      return memoized->second;
    }
  };
}

} // namespace neutu