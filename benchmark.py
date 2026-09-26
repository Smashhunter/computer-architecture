#!/usr/bin/env python3
import os
import sys

CSV_PATH = "benchmark.csv"

def main():
    pairs = []
    for line in sys.stdin:
        if ": " not in line:
            continue
        left, right = line.split(": ", 1)
        left = left.strip()
        right = right.strip()
        size = left.rsplit(" ", 2)[-2] + " KB"
        pairs.append((size, right))

    if not pairs:
        return

    header = ";".join(s for s, _ in pairs) + ";"
    row = ";".join(v for _, v in pairs) + ";"

    need_header = not os.path.exists(CSV_PATH) or os.path.getsize(CSV_PATH) == 0

    with open(CSV_PATH, "a") as f:
        if need_header:
            f.write(header + "\n")
        f.write(row + "\n")

if __name__ == "__main__":
    main()
