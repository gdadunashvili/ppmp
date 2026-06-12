#pragma once
#include <chrono>
#include <sstream>

namespace gdu {
class Timer {
    using clock      = std::chrono::high_resolution_clock;
    using time_point = clock::time_point;
    using duration   = clock::duration;
    time_point start_;
    explicit Timer(time_point start) : start_{start} {};

public:
    Timer() = delete;

    static Timer start() { return Timer(clock::now()); }

    duration time_elapsed() {
        const auto now          = clock::now();
        const auto time_elapsed = now - start_;
        return time_elapsed;
    }

    void time_elapsed_info() { human_readable_time(time_elapsed()); }

    static std::string human_readable_time(duration duration) {
        using std::chrono::duration_cast;

        using std::chrono::microseconds;
        using std::chrono::milliseconds;
        using std::chrono::nanoseconds;
        using std::chrono::seconds;

        using std::chrono::minutes;

        using std::chrono::hours;

        using std::chrono::days;

        std::stringstream info{"elapsed time: "};
        if (duration < nanoseconds{1000}) {
            info << duration_cast<nanoseconds>(duration);
        } else if (duration < microseconds{1000}) {
            info << duration_cast<microseconds>(duration) << "us (" << duration_cast<nanoseconds>(duration) << "ns)";
        } else if (duration < milliseconds{1000}) {
            info << duration_cast<milliseconds>(duration) << " (" << duration_cast<microseconds>(duration) << ")";
        } else if (duration < seconds{60}) {
            info << duration_cast<seconds>(duration) << " (" << duration_cast<milliseconds>(duration) << ")";
        } else if (duration < minutes{60}) {
            info << duration_cast<minutes>(duration) << " (" << duration_cast<seconds>(duration) << ")";
        } else if (duration < hours{24}) {
            info << duration_cast<hours>(duration) << " (" << duration_cast<minutes>(duration) << ")";
        } else {
            info << duration_cast<days>(duration) << " (" << duration_cast<hours>(duration) << ")";
        }
        return info.str();
    }
};

}  // namespace gdu
