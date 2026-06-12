#ifndef PPMP_FILE_HANDLER_H
#define PPMP_FILE_HANDLER_H

#include <bit>
#include <cstdint>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

class FileHandler {
    struct M {
        std::ofstream         file;
        std::filesystem::path filepath;
    } m;
    explicit FileHandler(M&& m_init) : m(std::move(m_init)) {};

public:
    [[nodiscard("Discarding a factory function")]]
    static FileHandler open_file(const std::filesystem::path& filepath, std::ios::openmode openmode) {
        auto file = std::ofstream(filepath, openmode);

        return FileHandler(M{.file = std::move(file), .filepath = filepath});
    };
    std::ofstream& get_file() { return m.file; }
    void           close_file() { m.file.close(); }

    void write_to_file(const std::vector<std::byte>& data) {
        if (m.file.is_open()) {

            // Onnly the api of write requires a char* and not a byte*. The only way to avoid bit_casting a pointer of
            // reinterpret_cast-ing a pointer is to loop throug the data and cast is bit by bit. A quick measurement
            // showed that this is an order of magnitude slower.
            // NOLINTNEXTLINE(bugprone-bitwise-pointer-cast)
            m.file.write(std::bit_cast<const char*>(data.data()), static_cast<std::int64_t>(data.size()));
        } else {
            std::cerr << "File is not open for writing\n";
        }
    }

    void write_to_file(std::string_view data) {
        if (m.file.is_open()) {
            const auto data_size = data.size();
            if (data_size > std::numeric_limits<std::streamsize>::max()) {
                std::cout << "Unhandled pahtological case: Provided Data is too large "
                             "to be streamed in one go! Terminating!"
                          << std::endl;
                std::terminate();
            }
            m.file.write(data.data(), static_cast<std::streamsize>(data.size()));
        } else {
            std::cerr << "File is not open for writing\n";
        }
    }
};

#endif  // PPMP_FILE_HANDLER_H
