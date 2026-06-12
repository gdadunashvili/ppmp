import subprocess
import sys
import filecmp


def main():
    if len(sys.argv) != 3:
        print(f"Usage: {sys.argv[0]} <demo_binary> <expected_ppm>",
              file=sys.stderr)
        return 1

    demo_binary = sys.argv[1]
    expected_ppm = sys.argv[2]

    result = subprocess.run([demo_binary], capture_output=True, text=True)

    if result.returncode != 0:
        print(f"FAIL: demo binary exited with code {result.returncode}")
        if result.stdout:
            print(f"stdout: {result.stdout}")
        if result.stderr:
            print(f"stderr: {result.stderr}")
        return 1

    generated_ppm = "example.ppm"
    if not filecmp.cmp(generated_ppm, expected_ppm, shallow=False):
        print(f"FAIL: {generated_ppm} differs from {expected_ppm}")
        return 1

    print("PASS")
    return 0


if __name__ == "__main__":
    sys.exit(main())
