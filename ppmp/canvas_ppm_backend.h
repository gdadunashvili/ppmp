#ifndef PPMP_CANVAS_PPM_BACKEND_H
#define PPMP_CANVAS_PPM_BACKEND_H

#include "ppmp/error.h"
#include "ppmp/rgb_color.h"

#include <cstddef>
#include <expected>
#include <iostream>
#include <sstream>
#include <utility>
#include <vector>

namespace ppmp {

/// \brief Implementation of the Canvas interface for PPM files.
class PPMCanvas {
    struct M {
        std::vector<std::byte> data;
        std::size_t            header_size;
        std::size_t            width;
        std::size_t            height;
    } m;

    explicit PPMCanvas(M&& m_init) : m{std::move(m_init)} {};

    constexpr static std::size_t PIXEL_WIDTH = 3;
    ErrorStashT                  ErrorStash{};

public:
    using Result = std::expected<void, Error>;

    /// \brief Create a blank canvas with the specified width and height and the specified background color.
    [[nodiscard("Discarding a factory function")]]
    static PPMCanvas blank(std::size_t width, std::size_t height, RGBColor background_color) {
        std::stringstream header{};
        header << "P6\n" << width << " " << height << "\n255\n";

        auto                   header_size = header.str().size();
        std::vector<std::byte> header_bytes{};
        header_bytes.reserve(header_size + (width * height * PIXEL_WIDTH));

        for (const auto& byte : header.str()) {
            header_bytes.push_back(std::byte{static_cast<unsigned char>(byte)});
        }

        for (std::size_t i = 0; i < width * height; ++i) {
            header_bytes.push_back(background_color.R);
            header_bytes.push_back(background_color.G);
            header_bytes.push_back(background_color.B);
        }

        return PPMCanvas(M{.data = header_bytes, .header_size = header_size, .width = width, .height = height});
    }

    /// \brief Colors a single pixel with a given color.
    /// \param x x-coordinate of the pixel to color, in canvas coordinates.
    /// \param y y-coordinate of the pixel to color, in canvas coordinates.
    /// \return  Error::OutOfBounds if the coordinates are out of bounds. This is a non-fatal error.
    ///
    /// Canvas coordinates start at the top left corner and go down and to the right.
    [[nodiscard("Returned error is being descarded.")]]
    Result pixel_shader(std::size_t x, std::size_t y, RGBColor color) {

        if (x >= m.width || y >= m.height) { return std::unexpected(Error::OutOfBounds); }

        const std::size_t i = m.header_size + (y * m.width + x) * PIXEL_WIDTH;
        // Bounds are already checked with the above if statement and i is calculated inside the bounds of the vector.
        // NOLINTBEGIN (cppcoreguidelines-pro-bounds-avoid-unchecked-container-access)
        m.data[i]     = std::byte{color.R};
        m.data[i + 1] = std::byte{color.G};
        m.data[i + 2] = std::byte{color.B};
        // NOLINTEND
        return {};
    }

    /// \brief Colors a quadrilateral with a given color. Uses a pixel shader to color each pixel in the quad.
    void quad_shader(std::size_t x, std::size_t y, std::size_t quad_width, std::size_t quad_height, RGBColor color) {

        for (std::size_t j = 0; j < quad_height; ++j) {
            for (std::size_t i = 0; i < quad_width; ++i) {
                ErrorStash = pixel_shader(x - quad_width / 2 + i, y - quad_height / 2 + j, color);
            }
        }
    }

    [[nodiscard]] const auto& get_data() const { return m.data; }
    [[nodiscard]] auto&       get_mut_data() { return m.data; }
    [[nodiscard]] std::size_t width() const { return m.width; }
    [[nodiscard]] std::size_t height() const { return m.height; }
};

}  // namespace ppmp

#endif  // PPMP_CANVAS_PPM_BACKEND_H
