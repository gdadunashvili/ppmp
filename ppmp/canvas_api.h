#ifndef PPMP_CANVAS_API_H
#define PPMP_CANVAS_API_H

#include "ppmp/rgb_color.h"

#include <concepts>
#include <cstddef>
#include <string_view>
#include <vector>

namespace ppmp {
class DataSaver;
/// \brief Interface definition for a canvas in the PPMP library
template <typename T>
concept Canvas = requires(T t, std::size_t integral_num, RGBColor color, std::string_view name, DataSaver data_saver) {
    { t.blank(integral_num, integral_num, color) } -> std::same_as<T>;
    { t.pixel_shader_unprotected(integral_num, integral_num, color) } -> std::same_as<void>;
    { t.quad_shader(integral_num, integral_num, integral_num, integral_num, color) } -> std::same_as<void>;
    { t.get_data() } -> std::same_as<const std::vector<std::byte>&>;
    { t.get_mut_data() } -> std::same_as<std::vector<std::byte>&>;
    { t.width() } -> std::same_as<std::size_t>;
    { t.height() } -> std::same_as<std::size_t>;
    { t.save(data_saver) } -> std::same_as<void>;
};

}  // namespace ppmp

#endif  // PPMP_CANVAS_API_H
