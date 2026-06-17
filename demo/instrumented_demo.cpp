#include "ppmp/error.h"
#include "ppmp/figure.h"
#include "ppmp/rgb_color.h"

#include "tools/code_utils/utils.h"

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
using ppmp::print_error_stack;

// constants
using ppmp::NAMED_COLORS;

}  // namespace

int main() {

    constexpr std::size_t scale = 1000;

    auto setup_timer = gdu::Timer::start();

    auto fig = PPMFigure::create_proportional(scale,
                                              PPMFigure::KWArgs{.color = {},
                                                                .x_min = -2_r * std::numbers::pi_v<Real>,
                                                                .x_max = 2_r * std::numbers::pi_v<Real>,
                                                                .y_min = {},
                                                                .y_max = {}});

    std::size_t points = 110;
    auto        xs     = linspace(0_r, .99_r, points);
    auto        xsb    = linspace(.99_r, 0_r, points);
    auto        mxs    = linspace(0_r, -.99_r, points);

    auto setup_time = setup_timer.time_elapsed();

    auto plotting_math_timer = gdu::Timer::start();

    fig.plot(
        xs, xsb, PlotParams{.brush_width = 15, .brush_color{NAMED_COLORS.black}, .plot_type = ppmp::PlotType::Line});
    fig.plot(
        xs, mxs, PlotParams{.brush_width = 15, .brush_color{NAMED_COLORS.black}, .plot_type = ppmp::PlotType::Line});

    fig.plot(xs, PlotParams{.brush_width = 10, .brush_color{NAMED_COLORS.green}, .plot_type = ppmp::PlotType::Line});

    auto sin_x = [](Real x) { return std::sin(x); };

    auto sin_sx = [](Real x) {
        static Real s = 2_r * std::numbers::pi_v<Real>;
        return std::sin(s * x);
    };

    auto cos_sx = [](Real x) {
        static Real s = 2_r * std::numbers::pi_v<Real>;
        return std::cos(s * x) - 2_r;
    };

    fig.plot(sin_x, 100, PlotParams{.brush_width = 14, .brush_color{}, .plot_type = ppmp::PlotType::Bar});
    fig.plot([](Real x) { return x * x * x / 4_r; },
             201,
             PlotParams{.brush_width = 10, .brush_color{}, .plot_type = ppmp::PlotType::Line});

    fig.parametric_plot(
        cos_sx, sin_sx, 101, PlotParams{.brush_width = 40, .brush_color{}, .plot_type = ppmp::PlotType::Line});

    auto plotting_math_time = plotting_math_timer.time_elapsed();
    std::cout << "Time elapsed for setup: " << gdu::Timer::human_readable_time(setup_time) << std::endl;
    std::cout << "Time elapsed for plotting math: " << gdu::Timer::human_readable_time(plotting_math_time) << std::endl;

    auto saving_timer = gdu::Timer::start();
    fig.save_canvas("example");
    std::cout << "Time elapsed for rendering: " << gdu::Timer::human_readable_time(saving_timer.time_elapsed())
              << std::endl;

    print_error_stack(ppmp::ErrorData::get_errors());
    std::cout << "Done." << std::endl;

    return 0;
}
