#ifndef SRC_FIGURE_H
#define SRC_FIGURE_H

#include "ppmp/canvas_api.h"
#include "ppmp/canvas_ppm_backend.h"
#include "ppmp/file_handler.h"
#include "ppmp/interfaces.h"
#include "ppmp/rgb_color.h"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <exception>
#include <ranges>

namespace ppmp {

template <typename T>
    requires std::is_floating_point_v<T>
inline auto linspace(T initial, T final, std::size_t num_points) {
    return std::views::iota(0UL, num_points) | std::views::transform([initial, final, num_points](std::size_t x) {
               auto normalized_x = static_cast<T>(x) / static_cast<T>(num_points - 1);
               auto scaled_x     = initial + normalized_x * (final - initial);
               return scaled_x;
           });
}

enum struct PlotType : std::uint8_t {
    Scatter = 0,
    Line,
    Bar,
};

enum struct AxesType : std::uint8_t {
    Linear = 0,
    LogarithmicX,
    LogarithmicY,
    LogLog,
};

struct PlotParams {
    std::size_t             brush_width{3};
    std::optional<RGBColor> brush_color;
    PlotType                plot_type{PlotType::Line};
    AxesType                axes_type{AxesType::Linear};
};
template <Canvas CanvasType>
class Figure {

public:
    struct M {
        CanvasType  canvas;
        std::size_t current_color_index{0};
        RGBColor    background_color{};
        Real        x_min{};
        Real        x_max{};
        Real        y_min{};
        Real        y_max{};
        struct PlotAxes {
            bool x;
            bool y;
        } plot_axes{};
        struct PlotBox {
            bool top;
            bool bottom;
            bool left;
            bool right;
        } plot_box{};
    } m;

    explicit Figure(M&& m_init) : m{std::move(m_init)} {};

    constexpr static std::array DEFAULT_COLORS{NAMED_COLORS.blue,
                                               NAMED_COLORS.green,
                                               NAMED_COLORS.cyan,
                                               NAMED_COLORS.red,
                                               NAMED_COLORS.magenta,
                                               NAMED_COLORS.yellow,
                                               NAMED_COLORS.turquoise,
                                               NAMED_COLORS.orange,
                                               NAMED_COLORS.brown,
                                               NAMED_COLORS.black};

    struct KWArgs {
        std::optional<RGBColor>                     color;
        std::optional<Real>                         x_min;
        std::optional<Real>                         x_max;
        std::optional<Real>                         y_min;
        std::optional<Real>                         y_max;
        std::optional<typename Figure::M::PlotAxes> plot_axes;
        std::optional<typename Figure::M::PlotBox>  plot_box;
    };

    constexpr static Figure create_proportional(std::size_t scale, KWArgs kwargs) {

        using ppmp::literals::operator""_r;
        auto x_max = kwargs.x_max.value_or(1_r);
        auto x_min = kwargs.x_min.value_or(-1_r);
        auto y_max = kwargs.y_max.value_or(1_r);
        auto y_min = kwargs.y_min.value_or(-1_r);

        auto dx = x_max - x_min;
        auto dy = y_max - y_min;

        auto width  = static_cast<std::size_t>(std::ceil(dx * scale));
        auto height = static_cast<std::size_t>(std::ceil(dy * scale));

        return Figure::create(width, height, kwargs);
    }

    [[nodiscard("Discarding a factory function")]]
    constexpr static Figure create(std::size_t width, std::size_t height) {
        return Figure::create(width, height, KWArgs{});
    }

    [[nodiscard("Discarding a factory function")]]
    constexpr static Figure create(std::size_t width, std::size_t height, KWArgs kwargs) {
        using ppmp::literals::operator""_r;

        auto figure = Figure(M{
            .canvas           = PPMData::blank(width, height, NAMED_COLORS.white),
            .background_color = kwargs.color.value_or(NAMED_COLORS.white),
            .x_min            = kwargs.x_min.value_or(-1_r),
            .x_max            = kwargs.x_max.value_or(1_r),
            .y_min            = kwargs.y_min.value_or(-1_r),
            .y_max            = kwargs.y_max.value_or(1_r),
            .plot_axes        = kwargs.plot_axes.value_or({.x = true, .y = true}),
            .plot_box = kwargs.plot_box.value_or({.top = false, .bottom = false, .left = false, .right = false}),
        });

        figure.draw_axes();

        return figure;
    }

    constexpr auto x_points(std::size_t points) { return linspace(m.x_min, m.x_max, points); }

    void plot(const Callable<Real> auto& y_transform, std::size_t points = 100, const PlotParams& p = PlotParams{}) {
        auto xs = x_points(points);
        auto ys = xs | std::views::transform([y_transform](Real x) { return y_transform(x); });

        plot(xs, ys, p);
    }

    // NOTE: this is a parametric polot. maybe should be renamed
    void plot(const Callable<Real> auto& x_transform,
              const Callable<Real> auto& y_transform,
              std::size_t                points,
              const PlotParams&          p = PlotParams{}) {

        using ppmp::literals::operator""_r;

        auto s  = linspace(0_r, 1_r, points);
        auto xs = s | std::views::transform([x_transform](Real x) { return x_transform(x); });

        auto ys = s | std::views::transform([y_transform](Real x) { return y_transform(x); });

        plot(xs, ys, p);
    }

    template <Container YContainer>
    void plot(const YContainer& ys, const PlotParams& p = PlotParams{}) {
        auto xs = x_points(ys.size());
        plot(xs, ys, p);
    }

    template <Container XContainer, Container YContainer>
    void plot(const XContainer& xs, const YContainer& ys, const PlotParams& p = PlotParams{}) {
        auto color = [this, &p]() -> RGBColor {
            if (p.brush_color.has_value()) { return p.brush_color.value(); }

            m.current_color_index = m.current_color_index % DEFAULT_COLORS.size();

            // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index) bounds were checked
            // NOLINTNEXTLINE (cppcoreguidelines-pro-bounds-avoid-unchecked-container-access)
            return DEFAULT_COLORS[m.current_color_index++];
        }();

        auto xs_ = xs | scale_to_frame_coord(m.x_min, m.x_max, m.canvas.width());
        auto ys_ = ys | scale_to_frame_coord(m.y_max, m.y_min, m.canvas.height());

        switch (p.plot_type) {
            case PlotType::Line: {
                for (const auto& [x, y, x_next, y_next] :
                     std::views::zip(xs_, ys_, xs_ | std::views::drop(1), ys_ | std::views::drop(1))) {
                    draw_line_in_canvas_coord(x, y, x_next, y_next, p.brush_width, color);
                }
                break;
            }

            case PlotType::Bar: {
                using ppmp::literals::operator""_r;
                auto y0 = scale_value_to_frame_coord(0, m.y_max, m.y_min, m.canvas.height());
                for (auto [x, y] : std::views::zip(xs_, ys_)) {
                    draw_line_in_canvas_coord(x, y0, x, y, p.brush_width, color);
                }
                break;
            }
            case PlotType::Scatter: {
                draw_batch(xs_, ys_, p.brush_width, color);
                break;
            }
            default: {
                std::cerr << "Invalid PlotType. Terminating";
                std::terminate();
            }
        }
    }

    void render_canvas(std::string_view filename) {
        std::string filename_{filename};
        std::string extension{};
        if constexpr (std::is_same_v<CanvasType, PPMData>) {
            extension  = ".ppm";
            filename_ += extension;
            auto file  = FileHandler::open_file(filename_, std::ios::out | std::ios::trunc | std::ios::binary);
            file.write_to_file(m.canvas.get_data());
        } else {
            std::cerr << "Can't save the given CanvasType. Terminating";
            std::terminate();
        }
    }

private:
    std::int64_t line(std::int64_t x, std::int64_t xs, std::int64_t xf, std::int64_t ys, std::int64_t yf) {
        return ((yf - ys) * x + ys * xf - yf * xs) / (xf - xs);
    }

    static std::size_t scale_value_to_frame_coord(Real val, Real min_val, Real max_val, std::size_t width) {
        const Real scale     = static_cast<Real>(width - 1) / (max_val - min_val);
        auto       scaled_el = static_cast<std::size_t>(scale * (val - min_val));
        return scaled_el;
    }

    constexpr static auto scale_to_frame_coord(Real min_val, Real max_val, std::size_t width) {
        return std::views::transform(
            [min_val, max_val, width](Real el) { return scale_value_to_frame_coord(el, min_val, max_val, width); });
    };

    void draw_line(Real        x_start,
                   Real        y_start,
                   Real        x_end,
                   Real        y_end,
                   std::size_t linewidth = 1,
                   RGBColor    color     = NAMED_COLORS.black) {

        // PERF: creating an array and piping it to a view might not be optimal
        auto xs  = std::array{x_start, x_end};
        auto ys  = std::array{y_start, y_end};
        auto xs_ = xs | scale_to_frame_coord(m.x_min, m.x_max, m.canvas.width());
        auto ys_ = ys | scale_to_frame_coord(m.y_max, m.y_min, m.canvas.height());

        // NOTE: we are putting the y coordinates backwards since the canvas data structure is indexed from top to
        // bottom but the y axes in the plot goes from bottom to top

        // NOLINTNEXTLINE (cppcoreguidelines-pro-bounds-avoid-unchecked-container-access)
        draw_line_in_canvas_coord(xs_[0], ys_[1], xs_[1], ys_[0], linewidth, color);
    }

    void draw_line_in_canvas_coord(std::int64_t x_start,
                                   std::int64_t y_start,
                                   std::int64_t x_end,
                                   std::int64_t y_end,
                                   std::int64_t linewidth,
                                   RGBColor     color) {

        auto w = static_cast<std::int64_t>(m.canvas.width());
        auto h = static_cast<std::int64_t>(m.canvas.height());
        // NOTE: clamping is not necessary for correctness but avoids unnecessary computation outside of visible area
        const auto xs = std::clamp(std::min(x_start, x_end), -1L, w);
        const auto xf = std::clamp(std::max(x_start, x_end), -1L, w);
        const auto dx = xf - xs;

        const auto ys = std::clamp(std::min(y_start, y_end), -1L, h);
        const auto yf = std::clamp(std::max(y_start, y_end), -1L, h);
        const auto dy = yf - ys;

        if (dx < dy) {
            for (std::int64_t y = ys; y <= yf; ++y) {
                const auto x = line(y, y_start, y_end, x_start, x_end);
                m.canvas.quad_shader(x, y, linewidth, linewidth, color);
            }
        }

        for (std::int64_t x = xs; x < xf; ++x) {
            const auto y = line(x, x_start, x_end, y_start, y_end);
            m.canvas.quad_shader(x, y, linewidth, linewidth, color);
        }
    }

    void draw_axes() {
        using ppmp::literals::operator""_r;
        if (m.plot_axes.x) { draw_line(m.x_min, 0_r, m.x_max, 0_r); }
        if (m.plot_axes.y) { draw_line(0_r, m.y_min, 0_r, m.y_max); }
        if (m.plot_box.top) { draw_line(m.x_min, m.y_max, m.x_max, m.y_max); }
        if (m.plot_box.bottom) { draw_line(m.x_min, m.y_min, m.x_max, m.y_min); }
        if (m.plot_box.left) { draw_line(m.x_min, m.y_min, m.x_min, m.y_max); }
        if (m.plot_box.right) { draw_line(m.x_max, m.y_min, m.x_max, m.y_max); }
    }

    template <Container XContainer, Container YContainer>
    void draw_batch(const XContainer& xs, const YContainer& ys, std::size_t linewidth, RGBColor color) {
        for (auto [x, y] : std::views::zip(xs, ys)) {
            m.canvas.quad_shader(x, y, linewidth, linewidth, color);
        }
    }
};

using PPMFigure = ppmp::Figure<ppmp::PPMData>;
}  // namespace ppmp
#endif  // SRC_FIGURE_H
