#ifndef PPMP_CANVAS_PPM_BACKEND_H
#define PPMP_CANVAS_PPM_BACKEND_H

#include "ppmp/data_saver.h"
#include "ppmp/rgb_color.h"

#include <algorithm>
#include <cstddef>
#include <cstring>
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

public:
    /// \brief Create a blank canvas with the specified width and height and the specified background color.
    [[nodiscard("Discarding a factory function")]]
    static PPMCanvas blank(std::size_t width, std::size_t height, RGBColor background_color) {
        std::stringstream header{};
        header << "P6\n" << width << " " << height << "\n255\n";

        auto                   header_size = header.str().size();
        std::vector<std::byte> data_bytes{};

        data_bytes.resize(header_size + (width * height * PIXEL_WIDTH));

        // NOTE: All these memcpys and fill_ns are performance optimizations. Sth more readable like the following looop
        // is roughly 25% (for arbitrary background_color)  and a bit more for the black background.
        // for (std::size_t i = 0; i < width * height; ++i) {
        //     data_bytes.push_back(background_color.R);
        //     data_bytes.push_back(background_color.G);
        //     data_bytes.push_back(background_color.B);
        // }

        // NOLINTNEXTLINE (cppcoreguidelines-pro-type-reinterpret-cast)
        std::memcpy(data_bytes.data(), reinterpret_cast<std::byte*>(header.str().data()), header_size);

        if (rgb_color_eq(background_color, ppmp::NAMED_COLORS.black)) {
            // bites are already set to 0 which is black
        } else {
            // NOLINTNEXTLINE (cppcoreguidelines-pro-type-reinterpret-cast)
            auto color_ptr_to_data_bytes = reinterpret_cast<RGBColor*>(data_bytes.data() + header_size);
            // NOLINTNEXTLINE (cppcoreguidelines-pro-bounds-pointer-arithmetic)
            std::fill_n(color_ptr_to_data_bytes, width * height, background_color);
        }

        return PPMCanvas(M{.data = data_bytes, .header_size = header_size, .width = width, .height = height});
    }

    /// \brief Colors a single pixel with a given color. This function DOES NOT perform boundschecks.
    /// \param x x-coordinate of the pixel to color, in canvas coordinates.
    /// \param y y-coordinate of the pixel to color, in canvas coordinates.
    ///
    /// Canvas coordinates start at the top left corner and go down and to the right.
    void pixel_shader_unprotected(std::size_t x, std::size_t y, RGBColor color) {

        const std::size_t i = m.header_size + (y * m.width + x) * PIXEL_WIDTH;
        // Bounds are already checked with the above if statement and i is calculated inside the bounds of the vector.
        // NOLINTBEGIN (cppcoreguidelines-pro-bounds-avoid-unchecked-container-access)
        m.data[i]     = std::byte{color.R};
        m.data[i + 1] = std::byte{color.G};
        m.data[i + 2] = std::byte{color.B};
        // NOLINTEND
    }

    /// \brief Colors a quadrilateral with a given color. Uses a pixel shader to color each pixel in the quad.
    void quad_shader(std::size_t x, std::size_t y, std::size_t quad_width, std::size_t quad_height, RGBColor color) {

        for (std::size_t j = 0; j < quad_height; ++j) {
            auto y_pos = y - quad_height / 2 + j;
            if (y_pos >= m.height) { continue; }

            for (std::size_t i = 0; i < quad_width; ++i) {
                const auto x_pos = x - quad_width / 2 + i;
                if (x_pos >= m.width) { continue; }
                pixel_shader_unprotected(x_pos, y_pos, color);
            }
        }
    }

    [[nodiscard]] const std::vector<std::byte>& get_data() const { return m.data; }
    [[nodiscard]] std::vector<std::byte>&       get_mut_data() { return m.data; }
    [[nodiscard]] std::size_t                   width() const { return m.width; }
    [[nodiscard]] std::size_t                   height() const { return m.height; }

    void save(DataSaver& ds) const { ds.save(*this); };
};

}  // namespace ppmp

#endif  // PPMP_CANVAS_PPM_BACKEND_H
