#include "ppmp/error.h"
#include "ppmp/figure.h"
#include "ppmp/rgb_color.h"

#include <cstddef>
#include <numbers>

namespace {

// types
using ppmp::PlotParams;
using ppmp::PPMFigure;
using ppmp::Real;

// functions
using ppmp::literals::operator""_r;
using ppmp::linspace;

// constants
using ppmp::NAMED_COLORS;

}  // namespace

int main() {

    constexpr std::size_t scale = 110;

    constexpr auto pi = std::numbers::pi_v<Real>;

    auto fig =
        PPMFigure::create_proportional(scale, PPMFigure::KWArgs{.color = {}, .x_min = -2_r * pi, .x_max = 2_r * pi});

    auto sin_x = [](Real x) { return std::sin(x); };

    auto sin_sx = [](Real x) {
        static Real s = 2_r * std::numbers::pi_v<Real>;
        return std::sin(s * x);
    };

    auto cos_sx = [](Real x) {
        static Real s = 2_r * std::numbers::pi_v<Real>;
        return std::cos(s * x) - 2_r;
    };

    // plot functions, x_axis is automatically generated between x_min and x_max
    fig.plot(sin_x, 90, PlotParams{.brush_width = 5, .brush_color{}, .plot_type = ppmp::PlotType::Bar});

    fig.plot([](Real x) { return x * x * x / 4_r; },
             201,
             PlotParams{.brush_width = 10, .brush_color{}, .plot_type = ppmp::PlotType::Line});

    // create a parametric plot with two functions, the parameter s is automatically generated between 0 and 1 with the
    // specied number of points
    fig.plot(cos_sx, sin_sx, 101, PlotParams{.brush_width = 5, .brush_color{}, .plot_type = ppmp::PlotType::Line});

    constexpr std::size_t points = 110;

    auto xs  = linspace(0_r, 0.99_r, points);
    auto xsb = linspace(0.99_r, 0_r, points);
    auto mxs = linspace(0_r, -0.99_r, points);

    // two data arrays
    fig.plot(
        xs, xsb, PlotParams{.brush_width = 5, .brush_color{NAMED_COLORS.black}, .plot_type = ppmp::PlotType::Line});
    fig.plot(
        xs, mxs, PlotParams{.brush_width = 5, .brush_color{NAMED_COLORS.black}, .plot_type = ppmp::PlotType::Line});

    // one data array (x coordinates are generated automatically between x_min and x_max)
    fig.plot(xs, PlotParams{.brush_width = 3, .brush_color{NAMED_COLORS.orange}, .plot_type = ppmp::PlotType::Line});

    fig.render_canvas("example");

    ppmp::print_error_stack(ppmp::ErrorData::get_errors());

    std::cout << "Done." << std::endl;

    return 0;
}
