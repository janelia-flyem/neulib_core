#ifndef CORE_SHAREDRESOURCEPOOL_H
#define CORE_SHAREDRESOURCEPOOL_H

#include <memory>
#include <queue>
#include <mutex>
#include <unordered_map>
#include <functional>

namespace neulib {

template<typename T>
class SharedResourcePool
{
public:
  SharedResourcePool(std::function<T*(void)> factory);
  std::shared_ptr<T> take();
  std::shared_ptr<T> take(std::function<T*(void)> factory);
  void add(const std::shared_ptr<T> &res);

  int count() const;

private:
  std::queue<std::shared_ptr<T>> m_resourceQueue;
  std::function<T*(void)> m_factory;
  std::mutex m_mutex;
};

template<typename T>
class SharedResourcePoolMap
{
public:
  SharedResourcePoolMap(std::function<T*(const std::string&)> factory);
  std::shared_ptr<T> take(const std::string &key);
  std::shared_ptr<T> take(
      const std::string &key, std::function<T*(const std::string&)> factory);
  void add(const std::string &key, const std::shared_ptr<T> &res);

private:
  std::unordered_map<std::string, std::shared_ptr<SharedResourcePool<T>>> m_resourceMap;
  std::function<T*(std::string)> m_factory;
  std::mutex m_mutex;
};

template<typename T>
class SharedResourceRetriever
{
public:
  SharedResourceRetriever(
      const std::shared_ptr<SharedResourcePoolMap<T>> &resourceMap,
      const std::string &key);
  ~SharedResourceRetriever();

  std::shared_ptr<T> get();

private:
  std::shared_ptr<SharedResourcePoolMap<T>> m_resourceMap;
  std::pair<std::string, std::shared_ptr<T>> m_resource;
};

}

template<typename T>
neulib::SharedResourcePool<T>::SharedResourcePool(
    std::function<T*(void)> factory)
{
  m_factory = factory;
}

template<typename T>
std::shared_ptr<T> neulib::SharedResourcePool<T>::take()
{
  return take(m_factory);
}

template<typename T>
std::shared_ptr<T> neulib::SharedResourcePool<T>::take(
    std::function<T*(void)> factory)
{
  std::lock_guard<std::mutex> guard(m_mutex);
  std::shared_ptr<T> res;
  if (!m_resourceQueue.empty()) {
    res = m_resourceQueue.front();
    m_resourceQueue.pop();
  }

  if (!res) {
    if (factory) {
      return std::shared_ptr<T>(factory());
    }
  }

  return res;
}

template<typename T>
void neulib::SharedResourcePool<T>::add(const std::shared_ptr<T> &res)
{
  if (res) {
    std::lock_guard<std::mutex> guard(m_mutex);
    m_resourceQueue.push(res);
  }
}

template<typename T>
int neulib::SharedResourcePool<T>::count() const
{
  return m_resourceQueue.size();
}


///////SharedResourcePoolMap///////

template<typename T>
neulib::SharedResourcePoolMap<T>::SharedResourcePoolMap(
    std::function<T*(const std::string&)> factory)
{
  m_factory = factory;
}

template<typename T>
std::shared_ptr<T> neulib::SharedResourcePoolMap<T>::take(const std::string &key)
{
  return take(key, m_factory);
}

template<typename T>
std::shared_ptr<T> neulib::SharedResourcePoolMap<T>::take(
    const std::string &key, std::function<T*(const std::string&)> factory)
{
  if (!key.empty()) {
    std::lock_guard<std::mutex> guard(m_mutex);
    if (m_resourceMap.count(key) == 0) {
      m_resourceMap[key] =
          std::shared_ptr<SharedResourcePool<T>>(new SharedResourcePool<T>(
            nullptr
          ));
    }

    return m_resourceMap.at(key)->take([key, factory]()->T* {
      return factory(key);
    });
  }

  return std::shared_ptr<T>();
}

template<typename T>
void neulib::SharedResourcePoolMap<T>::add(
    const std::string &key, const std::shared_ptr<T> &res)
{
  if (!key.empty()) {
    std::lock_guard<std::mutex> guard(m_mutex);
    if (m_resourceMap.count(key) == 0) {
      m_resourceMap[key] =
          std::shared_ptr<SharedResourcePool<T>>(
              new SharedResourcePool<T>([key, this]() { return m_factory(key); }));
    }

    m_resourceMap.at(key)->add(res);
  }
}


////////SharedResourceRetriever///////////
template<typename T>
neulib::SharedResourceRetriever<T>::SharedResourceRetriever(
    const std::shared_ptr<SharedResourcePoolMap<T>> &resourceMap,
    const std::string &key)
{
  m_resourceMap = resourceMap;
  m_resource.first = key;
}


template<typename T>
neulib::SharedResourceRetriever<T>::~SharedResourceRetriever()
{
  if (m_resourceMap) {
    if (m_resource.second) {
      m_resourceMap->add(m_resource.first, m_resource.second);
    }
  }
}

template<typename T>
std::shared_ptr<T> neulib::SharedResourceRetriever<T>::get()
{
  if (!m_resource.second) {
    m_resource.second = m_resourceMap->take(m_resource.first);
  }

  return m_resource.second;
}


#endif
