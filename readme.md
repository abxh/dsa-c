
# dsa-c

[doxygen documentation](https://abxh.github.io/dsa-c/) | ![tests](https://github.com/abxh/dsa-c/actions/workflows/tests.yml/badge.svg?event=push)

Generic, header-only and performant data structures. New memory allocation is kept to a minimum. Not thread-friendly.

All data types are expected to be Plain-Old-Datas (PODs). No explicit iterator mechanism is provided, but
macros can provide a primitive syntactical replacement.

Made for my own exploration and use.

Run `make test` to run all tests and examples. The `libsan` and `ubsan` sanitizers is required for building the tests.

Asserts are used to check various assumptions. Use `NDEBUG` flag to turn off asserts in release builds.

| **File**                                                                                          | Description                                              |                                                                                                                                                                      |
|---------------------------------------------------------------------------------------------------|----------------------------------------------------------|----------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| [fstack_template.h](https://github.com/abxh/dsa-c/blob/main/fstack/fstack_template.h)             | Fixed-size array-based stack                             | [Documentation](https://abxh.github.io/dsa-c/fstack__template_8h.html) [Example](https://github.com/abxh/dsa-c/blob/main/fstack/example/fstack_example.c)            |
| [fqueue_template.h](https://github.com/abxh/dsa-c/blob/main/fqueue/fqueue_template.h)             | Fixed-size queue based on ring buffer                    | [Documentation](https://abxh.github.io/dsa-c/fqueue__template_8h.html) [Example](https://github.com/abxh/dsa-c/blob/main/fqueue/example/fqueue_example.c)            |
| [fpqueue_template.h](https://github.com/abxh/dsa-c/blob/main/fpqueue/fpqueue_template.h)          | Fixed-size priority queue based on binary (max-)heap     | [Documentation](https://abxh.github.io/dsa-c/fpqueue__template_8h.html)  [Example](https://github.com/abxh/dsa-c/blob/main/fpqueue/example/fpqueue_example.c)        |
| [fhashtable_template.h](https://github.com/abxh/dsa-c/blob/main/fhashtable/fhashtable_template.h) | Fixed-size open-adressing hashtable (robin hood hashing) | [Documentation](https://abxh.github.io/dsa-c/fhashtable__template_8h.html) [Example](https://github.com/abxh/dsa-c/blob/main/fhashtable/example/fhashtable_example.c)|
| [rbtree_template.h](https://github.com/abxh/dsa-c/blob/main/rbtree/rbtree_template.h)             | Intrusive red-black tree                                 | [Documentation](https://abxh.github.io/dsa-c/rbtree__template_8h.html) [Example](https://github.com/abxh/dsa-c/blob/main/rbtree/example/rbtree_example.c)            |
| [arena_template.h](https://github.com/abxh/dsa-c/blob/main/arena/arena_template.h)                | Arena allocator                                          | [Documentation](https://abxh.github.io/dsa-c/arena__template_8h.html) [Example](https://github.com/abxh/dsa-c/blob/main/arena/example/arena_example.c)               |
