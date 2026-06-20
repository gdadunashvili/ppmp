#ifndef SRC_FIGURE_H
#define SRC_FIGURE_H

#include "ppmp/canvas_api.h"
#include "ppmp/canvas_ppm_backend.h"
#include "ppmp/data_saver.h"
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
using ppmp::literals::operator""_r;

/// \brief Generates a range of numbers linearly spaced between initial and final value (inclusive).
/// \tparam T Floating point type (e.g. float, double), underlying type of the range.
/// \param num_points number of points in the range
template <typename T>
    requires std::is_floating_point_v<T>
inline auto linspace(T initial, T final, std::size_t num_points) {
    return std::views::iota(0UL, num_points) | std::views::transform([initial, final, num_points](std::size_t x) {
               auto normalized_x = static_cast<T>(x) / static_cast<T>(num_points - 1);
               auto scaled_x     = initial + normalized_x * (final - initial);
               return scaled_x;
           });
}

/// \brief Type of the plot used during plot configuration.
enum struct PlotType : std::uint8_t {
    Scatter = 0,
    Line,
    Bar,
};

enum struct AxesScaling : std::uint8_t {
    Linear = 0,
    // gToDo: lacks implementation
    // LogarithmicX,
    // LogarithmicY,
    // LogLog,
};

// gToDo: redesign PlotParams, since omitting fields during initialization is frowned upon by the standard
struct PlotParams {
    std::size_t             brush_width{3};
    std::optional<RGBColor> brush_color;
    PlotType                plot_type{PlotType::Line};
    AxesScaling             axes_type{AxesScaling::Linear};
};
template <Canvas CanvasType>
class Figure {

public:
    /// \brief Data members of the figure. It is generally not intended for the user to direclty modify the members.
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

private:
    explicit Figure(M&& m_init) : m{std::move(m_init)} {};

public:
    /// \brief Default colors through which the line plots cycle, if no color is explicitely specified.
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

    /// \brief Keyword arguments for the figure constructor.
    struct KWArgs {
        // gToDo: redesign kwargs, since omitting fields during initialization is frowned upon by the standard
        std::optional<RGBColor>                     color;
        std::optional<Real>                         x_min;
        std::optional<Real>                         x_max;
        std::optional<Real>                         y_min;
        std::optional<Real>                         y_max;
        std::optional<typename Figure::M::PlotAxes> plot_axes;
        std::optional<typename Figure::M::PlotBox>  plot_box;
    };

    /// \brief Creates a figure with a proportionally scaled canvas. I.e. if the x-range is twice as large as the
    /// y-range, the canvas will be twice as large. in the x direction.
    ///
    /// \param scale Factor by which the canvas is scaled in each direction.
    /// \param kwargs Keyword arguments for the figure constructor.
    ///
    /// If x-range specified by kwargs.x_min=-1 and kwargs.x_max=1 is 2 and the scale is 1000, then the canvas will have
    /// 2000 pixels in the x direction. Same matho works for y-direction.
    constexpr static Figure create_proportional(std::size_t scale, KWArgs kwargs = {}) {

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

    /// \brief Create a canvas with the specified width and height.
    ///
    /// If the aspect ration of the canvas is not the same as the aspect ratio of the viewframe, i.e.
    /// `width/height != (x_max - x_min) / (y_max - y_min)`, the viewframe will be scaled to fit the canvas.
    /// Which will cause the figure to be stretched or squished.
    ///
    [[nodiscard("Discarding a factory function")]]
    constexpr static Figure create(std::size_t width, std::size_t height, KWArgs kwargs = {}) {

        auto color  = kwargs.color.value_or(NAMED_COLORS.white);
        auto figure = Figure(M{
            .canvas           = PPMCanvas::blank(width, height, color),
            .background_color = color,
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

    /// \brief Generates a range of numbers linearly spaced between x_min and x_max.
    constexpr auto x_points(std::size_t points) { return linspace(m.x_min, m.x_max, points); }

    /// \brief Plots a function y_transform(x) over the range of x points.
    /// \param y_transform Any c++ callable that takes a Real and returns a Real. Function templates are not supported.
    /// \param points Number of points in the x-range.
    void plot(const Callable<Real> auto& y_transform, std::size_t points = 100, const PlotParams& p = PlotParams{}) {
        auto xs = x_points(points);
        auto ys = xs | std::views::transform([y_transform](Real x) { return y_transform(x); });

        plot(xs, ys, p);
    }

    /// \brief Plots two functions x_transform(s) and y_transform(y) that vary with a comon parameter s, where s varies
    /// from 0 to 1 (inclusive).
    ///
    /// \param x_transform c++ callable that takes a Real and returns a Real. Function templates are not supported.
    /// \param y_transform c++ callable that takes a Real and returns a Real. Function templates are not supported.
    /// \param points number of points for the parameter s
    void parametric_plot(const Callable<Real> auto& x_transform,
                         const Callable<Real> auto& y_transform,
                         std::size_t                points,
                         const PlotParams&          p = PlotParams{}) {

        auto s  = linspace(0_r, 1_r, points);
        auto xs = s | std::views::transform([x_transform](Real x) { return x_transform(x); });

        auto ys = s | std::views::transform([y_transform](Real x) { return y_transform(x); });

        plot(xs, ys, p);
    }

    /// \brief Plotsa a range of y values, against an automatically generated range of x values.
    /// \param ys User provided data. The corresponding x values are automatically generated in range of x_min to x_max,
    /// and evenlly spaced for each point of ys.
    template <Container YContainer>
    void plot(const YContainer& ys, const PlotParams& p = PlotParams{}) {
        auto xs = x_points(ys.size());
        plot(xs, ys, p);
    }

    /// \brief Plots a range of x values against a range of y values. If the ranges are mismatched, the longer range is
    /// truncated to the length of the shorter range.
    template <Container XContainer, Container YContainer>
    void plot(const XContainer& xs, const YContainer& ys, const PlotParams& p = PlotParams{}) {
        auto color = [this, &p]() -> RGBColor {
            if (p.brush_color.has_value()) { return p.brush_color.value(); }

            m.current_color_index = m.current_color_index % DEFAULT_COLORS.size();

            // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index) bounds were checked
            // NOLINTNEXTLINE (cppcoreguidelines-pro-bounds-avoid-unchecked-container-access)
            return DEFAULT_COLORS[m.current_color_index++];
        }();

        auto xs_ = xs | scale_to_canvas_coordinates(m.x_min, m.x_max, m.canvas.width());
        auto ys_ = ys | scale_to_canvas_coordinates(m.y_max, m.y_min, m.canvas.height());

        switch (p.plot_type) {
            case PlotType::Line: {
                for (const auto& [x, y, x_next, y_next] :
                     std::views::zip(xs_, ys_, xs_ | std::views::drop(1), ys_ | std::views::drop(1))) {
                    draw_line_in_canvas_coord(x, y, x_next, y_next, p.brush_width, color);
                }
                break;
            }

            case PlotType::Bar: {
                auto y0 = scale_value_to_canvas_coordinate(0, m.y_max, m.y_min, m.canvas.height());
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

    /// \brief Saves teh canvas to a file which matches the canvas type of the fiugre.
    void save_canvas(std::string_view filename) { DataSaver::create(filename).save(m.canvas); }

private:
    std::int64_t line(std::int64_t x, std::int64_t xs, std::int64_t xf, std::int64_t ys, std::int64_t yf) {
        return ((yf - ys) * x + ys * xf - yf * xs) / (xf - xs);
    }

    static std::size_t scale_value_to_canvas_coordinate(Real val, Real min_val, Real max_val, std::size_t width) {
        const Real scale     = static_cast<Real>(width - 1) / (max_val - min_val);
        auto       scaled_el = static_cast<std::size_t>(scale * (val - min_val));
        return scaled_el;
    }

    constexpr static auto scale_to_canvas_coordinates(Real min_val, Real max_val, std::size_t width) {
        return std::views::transform([min_val, max_val, width](Real el) {
            return scale_value_to_canvas_coordinate(el, min_val, max_val, width);
        });
    };

    void draw_line(Real        x_start,
                   Real        y_start,
                   Real        x_end,
                   Real        y_end,
                   std::size_t linewidth = 1,
                   RGBColor    color     = NAMED_COLORS.black) {

        auto x_start_scaled = scale_value_to_canvas_coordinate(x_start, m.x_min, m.x_max, m.canvas.width());
        auto y_start_scaled = scale_value_to_canvas_coordinate(y_start, m.y_max, m.y_min, m.canvas.height());
        auto x_end_scaled   = scale_value_to_canvas_coordinate(x_end, m.x_min, m.x_max, m.canvas.width());
        auto y_end_scaled   = scale_value_to_canvas_coordinate(y_end, m.y_max, m.y_min, m.canvas.height());

        // We are putting the y coordinates backwards since the canvas data structure is indexed from top to bottom but
        // the y axes in the plot goes from bottom to top.
        // NOLINTNEXTLINE (readability-suspicious-call-argument)
        draw_line_in_canvas_coord(x_start_scaled, y_end_scaled, x_end_scaled, y_start_scaled, linewidth, color);
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

using PPMFigure = ppmp::Figure<ppmp::PPMCanvas>;
}  // namespace ppmp
#endif  // SRC_FIGURE_H
