load("@aspect_rules_lint//lint:clang_tidy.bzl", "lint_clang_tidy_aspect")
load("@aspect_rules_lint//lint:lint_test.bzl", "lint_test")

clang_tidy = lint_clang_tidy_aspect(
    binary = Label("@llvm_toolchain//:clang-tidy"),
    configs = [
        Label("//:.clang-tidy"),
    ],
    lint_target_headers = True,
    angle_includes_are_system = True,
    verbose = False,
    rule_kinds = ["cc_binary", "cc_library", "cc_test"],
)

clang_tidy_test = lint_test(aspect = clang_tidy)

clang_tidy_aspect = clang_tidy

