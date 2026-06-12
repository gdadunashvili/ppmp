#ifndef PPMP_ERROR_H
#define PPMP_ERROR_H

#include <algorithm>
#include <array>
#include <cstddef>
#include <expected>
#include <iostream>
#include <utility>

namespace ppmp {

// These enum values are used to index an array, this size_t is an appropreate type for them
// NOLINTNEXTLINE(performance-enum-size)
enum class Error : std::size_t { GenericError = 0, OutOfBounds, InvalidDimensions, FileError };

using ErrorContainer = std::array<std::size_t, 4>;

class ErrorData {

    ErrorContainer errors;
    ErrorData() = default;

public:
    static ErrorData& get_instance() {
        static ErrorData error_data{};
        return error_data;
    }
    static ErrorContainer& get_errors() { return get_instance().errors; }

    ErrorData(ErrorData&&)                 = delete;
    ErrorData& operator=(ErrorData&&)      = delete;
    ErrorData(const ErrorData&)            = delete;
    ErrorData& operator=(const ErrorData&) = delete;
    ~ErrorData()                           = default;
};

inline static void print_error_stack(const ErrorContainer& errors) {

    std::cout << "Error Counts: ";
    for (std::size_t i = 0; i < errors.size(); ++i) {
        std::cout << "Error " << std::to_underlying(static_cast<Error>(i)) << " occured " << errors.at(i) << std::endl;
    }
}

class ErrorStashT {

public:
    template <typename T>
    constexpr ErrorStashT& operator=(std::expected<T, Error>&& res) {
        if (!res) { ErrorData::get_errors().at(std::to_underlying(std::move(res).error()))++; }
        return *this;
    }

    constexpr ErrorStashT& operator=(Error&& e) {
        ErrorData::get_errors().at(std::to_underlying(e))++;
        return *this;
    }
};

}  // namespace ppmp
#endif  // PPMP_ERROR_H
