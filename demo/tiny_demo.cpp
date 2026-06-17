#include "ppmp/figure.h"

int main() {
    auto fig = ppmp::PPMFigure::create_proportional(1000);

    auto f = [](ppmp::Real x) { return x * x; };
    fig.plot(f);

    fig.save_canvas("tiny_example");
}
