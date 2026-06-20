#include "ppmp/figure.h"
#include "ppmp/canvas_ppm_backend.h"
#include "ppmp/interfaces.h"
#include "ppmp/rgb_color.h"

#include <catch2/catch_test_macros.hpp>

#include <vector>

namespace ppmp::test {

TEST_CASE("Default Constructor Tests ", "[figure]") {
    using ppmp::literals::operator""_r;

    SECTION("Default PlotParams is default constructed with expected values") {
        constexpr PlotParams params{};

        REQUIRE(params.brush_width == 3);
        REQUIRE_FALSE(params.brush_color.has_value());
        REQUIRE(params.plot_type == ppmp::PlotType::Line);
        REQUIRE(params.axes_type == ppmp::AxesScaling::Linear);
    }

    SECTION("Default Figure is default constructed with expected values") {
        constexpr auto scale = 1000;

        auto fig = PPMFigure::create_proportional(scale, PPMFigure::KWArgs{});

        const auto& m = fig.m;

        REQUIRE(m.canvas.width() == scale * (m.x_max - m.x_min));
        REQUIRE(m.canvas.height() == scale * (m.y_max - m.y_min));
        REQUIRE(m.current_color_index == 0);
        REQUIRE(rgb_color_eq(m.background_color, ppmp::NAMED_COLORS.white));
        REQUIRE(m.x_min == -1_r);
        REQUIRE(m.x_max == 1_r);
        REQUIRE(m.y_min == -1_r);
        REQUIRE(m.y_max == 1_r);
        REQUIRE(m.plot_axes.x == true);
        REQUIRE(m.plot_axes.y == true);
        REQUIRE(m.plot_box.top == false);
        REQUIRE(m.plot_box.bottom == false);
        REQUIRE(m.plot_box.left == false);
        REQUIRE(m.plot_box.right == false);
    }
}

TEST_CASE("Figure Plotting Tests ", "[figure]") {

    constexpr auto scale = 10;

    auto fig = Figure<ppmp::PPMCanvas>::create_proportional(scale, PPMFigure::KWArgs{});

    auto xs = std::vector<ppmp::Real>{0_r, 1_r, 2_r, 3_r};
    auto ys = std::vector<ppmp::Real>{0_r, 1_r, 2_r, 3_r, 4_r, 5_r};

    PlotParams params{};

    SECTION("Using plot(x,y) with containers of unequal size does not cause an error for Line plots") {
        params.plot_type = PlotType::Line;
        fig.plot(xs, ys, params);
    }
    SECTION("Using plot(x,y) with containers of unequal size does not cause an error for Bar plots") {
        params.plot_type = PlotType::Bar;
        fig.plot(xs, ys, params);
    }
    SECTION("Using plot(x,y) with containers of unequal size does not cause an error for Scatter plots") {
        params.plot_type = PlotType::Scatter;
        fig.plot(xs, ys, params);
    }
}

}  // namespace ppmp::test

// These explicit template instantiations force the compiler to emit code for template functions that are otherwise
// never called by the tests. This provides real coverage statistics.
namespace {

struct CoverageFunc {
    ppmp::Real operator()(ppmp::Real x) const noexcept { return x; }
};

}  // namespace

template void ppmp::Figure<ppmp::PPMCanvas>::plot<CoverageFunc>(const CoverageFunc&,
                                                                std::size_t,
                                                                const ppmp::PlotParams&);

template void ppmp::Figure<ppmp::PPMCanvas>::parametric_plot<CoverageFunc, CoverageFunc>(const CoverageFunc&,
                                                                                         const CoverageFunc&,
                                                                                         std::size_t,
                                                                                         const ppmp::PlotParams&);

template void ppmp::Figure<ppmp::PPMCanvas>::plot<std::vector<ppmp::Real>>(const std::vector<ppmp::Real>&,
                                                                           const ppmp::PlotParams&);

template void ppmp::Figure<ppmp::PPMCanvas>::plot<std::vector<ppmp::Real>, std::vector<ppmp::Real>>(
    const std::vector<ppmp::Real>&,
    const std::vector<ppmp::Real>&,
    const ppmp::PlotParams&);

template void ppmp::Figure<ppmp::PPMCanvas>::draw_batch<std::vector<std::size_t>, std::vector<std::size_t>>(
    const std::vector<std::size_t>&,
    const std::vector<std::size_t>&,
    std::size_t,
    ppmp::RGBColor);

template class ppmp::Figure<ppmp::PPMCanvas>;
