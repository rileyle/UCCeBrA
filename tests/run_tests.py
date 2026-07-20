# tests/run_tests.py
#
# Entry point for the UCCeBrA test suite.
# Discovers and runs all three test modules in order:
#   1. Smoke tests       — binary runs, output file created
#   2. Functional tests  — format, counts, physics sanity
#   3. Benchmark tests   — event rate and event count logging
#
# Usage (run from repository root):
#   python tests/run_tests.py           # run all suites
#   python tests/test_smoke.py          # run smoke tests only
#   python tests/test_functional.py     # run functional tests only
#   python tests/test_benchmark.py      # run benchmark tests only

import os
import sys
import unittest

# Ensure the repository root is on the path so
# 'from tests.output_parser import ...' works regardless of how this
# script is invoked.
REPO_ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
if REPO_ROOT not in sys.path:
    sys.path.insert(0, REPO_ROOT)

import tests.test_smoke
import tests.test_functional
import tests.test_benchmark


def run_suite(module, label):
    """
    Load and run all tests from a module.
    Prints a one-line result summary with pass/fail counts.
    Returns (n_passed, n_total, was_successful).
    """
    loader = unittest.TestLoader()
    suite  = loader.loadTestsFromModule(module)
    n_total = suite.countTestCases()

    # Suppress per-test dot output; we print our own summary line
    devnull = open(os.devnull, "w")
    runner  = unittest.TextTestRunner(verbosity=0, stream=devnull)
    result  = runner.run(suite)
    devnull.close()

    n_passed = n_total - len(result.failures) - len(result.errors)
    ok = result.wasSuccessful()
    status = "OK" if ok else "FAILED"
    print(f"{label:<25} {status} ({n_passed}/{n_total})")

    # Print a brief description of any failures or errors
    for test, traceback in result.failures + result.errors:
        print(f"  FAIL: {test}")
        # Show only the final assertion line to keep output readable
        lines = [l for l in traceback.strip().splitlines() if l.strip()]
        if lines:
            print(f"        {lines[-1]}")

    return n_passed, n_total, ok


def main():
    # Change to repo root so relative paths in test modules resolve correctly
    os.chdir(REPO_ROOT)

    print()
    print("UCCeBrA Test Suite")
    print("=" * 40)

    suites = [
        (tests.test_smoke,       "Smoke tests"),
        (tests.test_functional,  "Functional tests"),
        (tests.test_benchmark,   "Benchmark tests"),
    ]

    total_passed = 0
    total_tests  = 0
    all_ok       = True

    for module, label in suites:
        n_passed, n_total, ok = run_suite(module, label)
        total_passed += n_passed
        total_tests  += n_total
        all_ok = all_ok and ok

    print()

    # Show paths to output files if they exist
    benchmark_log = os.path.join(REPO_ROOT, "tests", "benchmark.log")
    counts_json   = os.path.join(REPO_ROOT, "tests", "benchmarks", "event-counts.json")
    if os.path.exists(benchmark_log):
        print(f"Benchmark log:   tests/benchmark.log")
    if os.path.exists(counts_json):
        print(f"Event counts:    tests/benchmarks/event-counts.json")
    print()

    if all_ok:
        print(f"All tests passed ({total_passed}/{total_tests}).")
        sys.exit(0)
    else:
        print(f"FAILED: {total_tests - total_passed} test(s) failed.")
        sys.exit(1)


if __name__ == "__main__":
    main()
