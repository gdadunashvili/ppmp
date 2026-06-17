#include "ppmp/data_saver.h"

#include <catch2/catch_test_macros.hpp>

#include <filesystem>

namespace ppmp::test {

TEST_CASE("check_and_fix_extension a test", "[data_saver]") {

    auto filename_root      = std::string{"test"};
    auto expected_extension = std::string{".blabla"};
    auto expected_filename  = std::filesystem::path{filename_root + expected_extension};

    SECTION("check_and_fix_extension detects wrong extension and changes it to the expected one") {
        auto filename_with_wrong_extension = std::filesystem::path{filename_root + ".bla"};
        auto updated_filename = ppmp::check_and_fix_extension(filename_with_wrong_extension, expected_extension);

        CHECK(updated_filename == expected_filename);
    }

    SECTION("check_and_fix_extension detects no extension and appends the expected one") {
        auto filename_without_extension = std::filesystem::path{filename_root};
        auto updated_filename           = ppmp::check_and_fix_extension(filename_without_extension, expected_extension);

        CHECK(updated_filename == expected_filename);
    }

    SECTION("check_and_fix_extension returns same filename if a filename wiht correct extension was passed") {
        auto updated_filename = ppmp::check_and_fix_extension(expected_filename, expected_extension);

        CHECK(updated_filename == expected_filename);
    }
}

}  // namespace ppmp::test
