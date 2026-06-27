load("@rules_cc//cc:cc_binary.bzl", "cc_binary")

cc_binary(
    name = "demo",
    srcs = ["demo.cpp"],
    copts = ["-Wno-error=missing-field-initializers"],
    visibility = ["//test:__subpackages__"],
    deps = [
        "//ppmp:figure",
        "//tools/code_utils:utils",
    ],
)

cc_binary(
    name = "instrumented_demo",
    srcs = ["instrumented_demo.cpp"],
    copts = ["-Wno-error=missing-field-initializers"],
    deps = [
        "//ppmp:figure",
        "//tools/code_utils:utils",
    ],
)

cc_binary(
    name = "tiny_demo",
    srcs = ["tiny_demo.cpp"],
    copts = ["-Wno-error=missing-field-initializers"],
    deps = [
        "//ppmp:figure",
    ],
)

filegroup(
    name = "example_ppm",
    srcs = ["golden_example.ppm"],
    visibility = ["//visibility:public"],
)
