import sys
import filecmp


def main():
    if len(sys.argv) != 3:
        print(f"Usage: {sys.argv[0]} <demo_binary> <expected_ppm>",
              file=sys.stderr)
        return 1

    generated_ppm = sys.argv[1]
    expected_ppm = sys.argv[2]

    if not filecmp.cmp(generated_ppm, expected_ppm, shallow=True):
        print(f"FAIL: {generated_ppm} differs from {expected_ppm}")
        return 1
    print("PASS")
    return 0


if __name__ == "__main__":
    sys.exit(main())
