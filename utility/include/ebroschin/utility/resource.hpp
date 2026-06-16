#pragma once

#include <memory>
#include <functional>

namespace ebroschin::utility {

template<typename TValue>
class Resource {
public:
  using ValueType = TValue;

  Resource(const Resource& other) = delete;
  Resource& operator=(const Resource& other) = delete;
  Resource(Resource&& other) = default;
  Resource& operator=(Resource&& other) = default;

  TValue* Get() const noexcept;
  TValue* operator->() const noexcept;
  operator TValue*() const noexcept;
  TValue& operator*() const noexcept;

protected:
  Resource(std::function<TValue*()> createFunction, std::function<void(TValue*)> deleteFunction);

  std::unique_ptr<TValue, std::function<void(TValue*)>> pointer_;
};

template<typename TValue>
Resource<TValue>::Resource(std::function<TValue*()> createFunction, std::function<void(TValue*)> deleteFunction):
  pointer_{createFunction(), deleteFunction} { }

template<typename TValue>
TValue* Resource<TValue>::Get() const noexcept { return pointer_.get(); }

template<typename TValue>
TValue* Resource<TValue>::operator->() const noexcept { return Get(); }

template<typename TValue>
Resource<TValue>::operator TValue*() const noexcept { return Get(); }

template<typename TValue>
TValue& Resource<TValue>::operator*() const noexcept { return *Get(); }

}