#ifndef PPMP_INTERFACED_H
#define PPMP_INTERFACED_H

#include <concepts>
#include <ranges>

namespace ppmp {

using Real = float;

namespace literals {

// NOLINTNEXTLINE(google-runtime-float) this is the standard way of defining numeric literals
constexpr Real operator""_r(long double value) { return static_cast<Real>(value); }
constexpr Real operator""_r(unsigned long long value) { return static_cast<Real>(value); }

}  // namespace literals

template <typename C>
concept Container = std::ranges::input_range<C>;

template <typename CallableType, typename ArgumentType>
concept Callable = requires(CallableType f, ArgumentType x) {
    { f(x) } -> std::same_as<ArgumentType>;
};

}  // namespace ppmp
#endif  // PPMP_INTERFACED_H
