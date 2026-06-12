#ifndef PPMP_CANVAS_API_H
#define PPMP_CANVAS_API_H

#include "ppmp/error.h"
#include "ppmp/rgb_color.h"

#include <concepts>
#include <cstddef>
#include <expected>
#include <vector>

namespace ppmp {
template <typename T>
concept Canvas = requires(T t, std::size_t integral_num, RGBColor color) {
    { t.blank(integral_num, integral_num, color) } -> std::same_as<T>;
    { t.pixel_shader(integral_num, integral_num, color) } -> std::same_as<std::expected<void, Error>>;
    { t.quad_shader(integral_num, integral_num, integral_num, integral_num, color) } -> std::same_as<void>;
    { t.get_data() } -> std::same_as<const std::vector<std::byte> &>;
    { t.get_mut_data() } -> std::same_as<std::vector<std::byte> &>;
    { t.width() } -> std::same_as<std::size_t>;
    { t.height() } -> std::same_as<std::size_t>;
} && std::same_as<typename T::Result, std::expected<void, Error>>;

}  // namespace ppmp

#endif  // PPMP_CANVAS_API_H
