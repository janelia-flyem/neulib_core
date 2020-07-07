#ifndef NEULIB_CORE_UTILITIES_H
#define NEULIB_CORE_UTILITIES_H

#include <string>
#include <map>
#include <mutex>
#include <tuple>
#include <functional>
#include <memory>

namespace neulib
{

template <typename...>
struct voider { using type = void; };

template <typename... T>
using void_t = typename voider<T...>::type;

template <typename T>
void Assign(T *out, const T &v)
{
  if (out != NULL) {
    *out = v;
  }
}

template<typename T>
struct ToUnsignedType {
};

template<>
struct ToUnsignedType<int>
{
  using type = unsigned int;
};

template<>
struct ToUnsignedType<int64_t>
{
  using type = uint64_t;
};

template <typename T>
typename ToUnsignedType<T>::type UnsignedCrop(const T &v)
{
  if (v < 0) {
    return typename ToUnsignedType<T>::type(0);
  }

  return typename ToUnsignedType<T>::type(v);
}

std::string ToString(const void *p);

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

template <typename T, typename... ArgTypes>
std::function<T(ArgTypes...)> Memoize(T (*fn)(ArgTypes...))
{
  std::shared_ptr<std::mutex> mutex(new std::mutex);
  std::map<std::tuple<ArgTypes...>, T> cache;
  return [fn, cache, mutex](ArgTypes... args) mutable -> T {
    std::lock_guard<std::mutex> guard(*mutex);

    auto argTuple = std::make_tuple(args...);

    auto memoized = cache.find(argTuple);
    if (memoized == cache.end()) {
      return (cache[argTuple] = fn(args...));
    } else {
      return memoized->second;
    }
  };
}

//A convenient function for const-referenced parameters, which can break Memoize.
template <typename T, typename A>
std::function<T(const A&)> SingleConstParameterMemoize(T (*fn)(const A&))
{
  std::map<A, T> cache;
  std::shared_ptr<std::mutex> mutex(new std::mutex);
  return [fn, cache, mutex](const A &key) mutable -> T {
    std::lock_guard<std::mutex> guard(*mutex);

    auto memoized = cache.find(key);
    if (memoized == cache.end()) {
      return (cache[key] = fn(key));
    } else {
      return memoized->second;
    }
  };
}

} // namespace neulib

#endif