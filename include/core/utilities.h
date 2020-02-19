#ifndef NEULIB_CORE_UTILITIES_H
#define NEULIB_CORE_UTILITIES_H

#include <string>
#include <map>
#include <mutex>
#include <tuple>
#include <functional>

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

template<typename T>
inline T ClipValue(const T &v, const T &lower, const T&upper)
{
  return (v < lower) ? lower : (v > upper) ? upper : v;
}

template<typename T>
inline bool ClipRange(const T &lower, const T&upper, T &x0, T &x1)
{
  if (x0 <= x1) {
    if (x0 <= upper && x1 >= lower) {
      if (x0 < lower) {
        x0 = lower;
      }
      if (x1 > upper) {
        x1 = upper;
      }
      return true;
    }
  }

  return false;
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

} // namespace neulib

#endif