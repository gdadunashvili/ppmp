load("@rules_cc//cc:cc_test.bzl", "cc_test")
load("@rules_python//python:defs.bzl", "py_test")
load("//:tools/lint/linters.bzl", "clang_tidy_test")

cc_test(
    name = "data_saver_test",
    srcs = ["data_saver_test.cpp"],
    defines = ["CATCH_CONFIG_MAIN"],
    tags = ["unit"],
    deps = [
        "//ppmp:figure",
        "@catch2//:catch2_main",
    ],
)

cc_test(
    name = "figure_test",
    srcs = ["figure_test.cpp"],
    defines = ["CATCH_CONFIG_MAIN"],
    tags = ["unit"],
    deps = [
        "//ppmp:figure",
        "@catch2//:catch2_main",
    ],
)

clang_tidy_test(
    name = "figure_test_lint",
    srcs = ["//ppmp:figure"],
)

py_test(
    name = "integration_test",
    srcs = ["integration_test.py"],
    args = [
        "$(location //demo:demo)",
        "$(location //demo:example_ppm)",
    ],
    tags = ["integration"],
    data = [
        "//demo",
        "//demo:example_ppm",
    ],
)
