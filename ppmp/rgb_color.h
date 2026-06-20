#ifndef PPMP_RGB_COLOR_H
#define PPMP_RGB_COLOR_H

#include <cstddef>
#include <cstdint>

namespace ppmp {
struct RGBColor {
    std::byte R{};
    std::byte G{};
    std::byte B{};
};

constexpr auto RGBFromInts(std::uint32_t R, std::uint32_t G, std::uint32_t B) {
    return RGBColor{.R = static_cast<std::byte>(R), .G = static_cast<std::byte>(G), .B = static_cast<std::byte>(B)};
}

constexpr bool rgb_color_eq(const ppmp::RGBColor& lhs, const ppmp::RGBColor& rhs) {
    return (lhs.R == rhs.R) && (lhs.G == rhs.G) && (lhs.B == rhs.B);
}

constexpr struct NamedColors {
    RGBColor black = RGBFromInts(0, 0, 0);
    RGBColor white = RGBFromInts(255, 255, 255);

    RGBColor red   = RGBFromInts(255, 0, 0);
    RGBColor green = RGBFromInts(0, 255, 0);
    RGBColor blue  = RGBFromInts(0, 0, 255);

    RGBColor yellow  = RGBFromInts(255, 255, 0);
    RGBColor magenta = RGBFromInts(255, 0, 255);
    RGBColor cyan    = RGBFromInts(0, 255, 255);

    RGBColor turquoise = RGBFromInts(00, 150, 255);

    RGBColor orange = RGBFromInts(255, 205, 0);
    RGBColor brown  = RGBFromInts(255, 180, 90);
    RGBColor gold   = RGBFromInts(255, 220, 35);

} NAMED_COLORS{};

}  // namespace ppmp
#endif  // PPMP_RGB_COLOR_H
