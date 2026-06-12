import os
import subprocess
import sys

def find_runfiles_root():
    path = os.path.abspath(sys.argv[0])
    parts = path.split(os.sep)
    for i, part in enumerate(parts):
        if part.endswith(".runfiles"):
            return os.sep.join(parts[:i + 1])
    return None

def find_file(runfiles_root, name):
    for entry in os.listdir(runfiles_root):
        if entry in ("_main", "__init__.py", "MANIFEST", "_repo_mapping"):
            continue
        candidate = os.path.join(runfiles_root, entry, "bin", name)
        if os.path.isfile(candidate):
            return candidate
    return None

def find_dir(runfiles_root, suffix):
    for entry in os.listdir(runfiles_root):
        path = os.path.join(runfiles_root, entry, suffix)
        if os.path.isdir(path):
            return path
    return None

def main():
    workspace_dir = os.environ.get("BUILD_WORKSPACE_DIRECTORY")
    if not workspace_dir:
        print("Error: BUILD_WORKSPACE_DIRECTORY is not set. Run with 'bazel run //:coverage'.", file=sys.stderr)
        return 1

    runfiles = find_runfiles_root()
    if not runfiles:
        print(f"Error: cannot find .runfiles directory from argv0={sys.argv[0]}", file=sys.stderr)
        return 1

    coverage_dat = os.path.join(workspace_dir, "bazel-testlogs", "test", "figure_test", "coverage.dat")
    coverage_html = os.path.join(workspace_dir, "coverage_html")

    if not os.path.isfile(coverage_dat):
        print(f"Error: coverage data not found at {coverage_dat}", file=sys.stderr)
        print(file=sys.stderr)
        print("Run coverage collection first:", file=sys.stderr)
        print("  bazel coverage //test:figure_test", file=sys.stderr)
        return 1

    perl = find_file(runfiles, "perl")
    genhtml = find_file(runfiles, "genhtml")

    if not perl:
        print("Error: cannot find hermetic perl in runfiles", file=sys.stderr)
        return 1

    if not genhtml:
        print("Error: cannot find genhtml in runfiles", file=sys.stderr)
        return 1

    print("Generating HTML coverage report...")
    result = subprocess.run(
        [perl, genhtml, "--branch-coverage", "-o", coverage_html, coverage_dat],
        cwd=workspace_dir,
    )
    if result.returncode != 0:
        print(f"Error: genhtml failed with exit code {result.returncode}", file=sys.stderr)
        return result.returncode

    print(f"\nCoverage report generated at: {coverage_html}/index.html")
    return 0

if __name__ == "__main__":
    sys.exit(main())
