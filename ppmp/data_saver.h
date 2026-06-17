#ifndef PPMP_DATA_SAVER_H
#define PPMP_DATA_SAVER_H

#include "ppmp/canvas_api.h"
#include "ppmp/file_handler.h"

namespace ppmp {

inline static std::filesystem::path check_and_fix_extension(std::filesystem::path filename,
                                                            std::string_view      expected_extension) {

    if (auto extension = filename.extension(); extension != expected_extension) {
        std::cout << "Filename has unexpected extension: " << extension << ". This will be replaced by "
                  << expected_extension << "." << std::endl;
        return filename.replace_extension(expected_extension);
    }
    return filename;
};

class DataSaver {
    struct M {
        std::filesystem::path filename;
        FileHandler           file;
    } m;

public:
    static DataSaver create(const std::filesystem::path& filename) {
        const auto checked_filename =
            std::filesystem::path{check_and_fix_extension(filename, std::string_view{".ppm"})};

        return DataSaver(
            M{.filename = checked_filename,
              .file = FileHandler::open_file(checked_filename, std::ios::out | std::ios::trunc | std::ios::binary)});
    };

    void save(const Canvas auto& canvas) { m.file.write_to_file(canvas.get_data()); };

private:
    explicit DataSaver(M&& m_init) : m{std::move(m_init)} {};
};

}  // namespace ppmp

#endif  // PPMP_DATA_SAVER_H
