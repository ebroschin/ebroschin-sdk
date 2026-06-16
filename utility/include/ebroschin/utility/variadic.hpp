#pragma once

#include <cstdint>
#include <type_traits>

namespace ebroschin::utility {

//compile time check. when this template is instantiated, an unexpected case occurred
template<typename T>
static constexpr std::size_t IndexOf() {
  static_assert(!std::same_as<T, T>, "Unable to find index of given type");
  return 0;
}

template<typename T, typename U, typename... Us>
static constexpr std::size_t IndexOf() {
  if constexpr (std::same_as<T, U>) return 0;
  else return 1 + IndexOf<T, Us...>();
}

template <typename TTuple>
struct ForEachType;

template <typename... TTypes>
struct ForEachType<std::tuple<TTypes...>> {
  template <typename TFunction>
  static void Apply(TFunction&& function) {
    (function.template operator()<TTypes>(), ...);
  }
};

}