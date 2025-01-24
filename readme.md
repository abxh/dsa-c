
# dsa-c

![tests](https://github.com/abxh/dsa-c/actions/workflows/tests.yml/badge.svg?event=push)

Generic, header-only and performant data structures. New memory allocation is kept to a minimum. Not thread-friendly.

All data types are expected to be Plain-Old-Datas (PODs). No explicit iterator mechanism is provided, but
macros provides a primitive syntactical replacement.

Run `make test` to run all tests and examples. The `libsan` and `ubsan` sanitizers is required for building the tests.

Asserts are used to check various assumptions. Use `NDEBUG` flag to turn off asserts in release builds.
