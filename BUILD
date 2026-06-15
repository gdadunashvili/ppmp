load("@aspect_rules_lint//format:defs.bzl", "format_multirun", "format_test")

exports_files([".clang-tidy"])

format_multirun(
    name = "format",
    cc = "@clang_format//:executable",
    starlark = "@buildifier_prebuilt//:buildifier",
)

format_test(
    name = "format_test",
    cc = "@clang_format//:executable",
    no_sandbox = True,
    starlark = "@buildifier_prebuilt//:buildifier",
    target_compatible_with = ["@platforms//os:linux"],
    workspace = "//:LICENSE",
)

alias(
    name = "coverage",
    actual = "//tools/coverage:coverage",
)
