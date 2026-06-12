#include "ppmp/figure.h"

int main() {
    using ppmp::PPMFigure;  // The ploting class
    using ppmp::Real;       // ppmp internal alias to a flating pint number

    constexpr std::size_t pixel_scale = 1000;

    auto fig = PPMFigure::create_proportional(pixel_scale, PPMFigure::KWArgs{});

    auto f = [](Real x) { return x * x; };

    fig.plot(f);
    fig.render_canvas("tiny_example");
}
