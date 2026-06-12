load("@rules_cc//cc:cc_test.bzl", "cc_test")
load("@rules_python//python:defs.bzl", "py_test")
load("//:tools/lint/linters.bzl", "clang_tidy_test")

cc_test(
    name = "figure_test",
    srcs = ["figure_test.cpp"],
    defines = ["CATCH_CONFIG_MAIN"],
    deps = [
        "//ppmp:figure",
        "@catch2//:catch2_main",
    ],
)

clang_tidy_test(
    name = "figure_test_lint",
    srcs = [":figure_test"],
)

py_test(
    name = "integration_test",
    srcs = ["integration_test.py"],
    data = [
        "//demo:demo",
        "//demo:example_ppm",
    ],
    args = [
        "$(location //demo:demo)",
        "$(location //demo:example_ppm)",
    ],
)
