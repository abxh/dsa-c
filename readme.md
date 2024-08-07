# data-structures-c

[doxygen documentation](https://abxh.github.io/data-structures-c/)
 | ![tests](https://github.com/abxh/data-structures-c/actions/workflows/tests.yml/badge.svg?event=push)

Generic, header-only and performant data structures. New memory allocation is kept to a minimum.

Made for my own exploration and use.

Run `make test` to run all tests and examples.

Asserts are used to check various assumptions. Use `NDEBUG` flag to turn off asserts in release builds.

| **File**                                                                             | Description                                              |                                                                                                                                                                               |
|--------------------------------------------------------------------------------------|----------------------------------------------------------|-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| [fstack.h](https://github.com/abxh/data-structures-c/blob/main/lib/fstack.h)         | Fixed-size array-based stack                             | [Documentation](https://abxh.github.io/data-structures-c/fstack_8h.html) [Example](https://github.com/abxh/data-structures-c/blob/main/examples/fstack/fstack.c)              |
| [fqueue.h](https://github.com/abxh/data-structures-c/blob/main/lib/fqueue.h)         | Fixed-size queue based on ring buffer                    | [Documentation](https://abxh.github.io/data-structures-c/fqueue_8h.html) [Example](https://github.com/abxh/data-structures-c/blob/main/examples/fqueue/fqueue.c)              |
| [fpqueue.h](https://github.com/abxh/data-structures-c/blob/main/lib/fpqueue.h)       | Fixed-size priority queue based on binary heap           | [Documentation](https://abxh.github.io/data-structures-c/fpqueue_8h.html) [Example](https://github.com/abxh/data-structures-c/blob/main/examples/fpqueue/fpqueue.c)           |
| [fhashtable.h](https://github.com/abxh/data-structures-c/blob/main/lib/fhashtable.h) | Fixed-size open-adressing hashtable (robin hood hashing) | [Documentation](https://abxh.github.io/data-structures-c/fhashtable_8h.html) [Example](https://github.com/abxh/data-structures-c/blob/main/examples/fhashtable/fhashtable.c)  |
| [arena.h](https://github.com/abxh/data-structures-c/blob/main/lib/arena.h)           | Arena allocator                                          | [Documentation](https://abxh.github.io/data-structures-c/arena_8h.html) [Example](https://github.com/abxh/data-structures-c/blob/main/examples/arena/char_array.c)            |
| [list.h](https://github.com/abxh/data-structures-c/blob/main/lib/list.h)             | Intrusive circular doubly linked list                    | [Documentation](https://abxh.github.io/data-structures-c/list_8h.html) [Example](https://github.com/abxh/data-structures-c/blob/main/examples/list/list_example.c)            |
| [rbtree.h](https://github.com/abxh/data-structures-c/blob/main/lib/rbtree.h)         | Intrusive red-black tree                                 | [Documentation](https://abxh.github.io/data-structures-c/rbtree_8h.html)  [Example](https://github.com/abxh/data-structures-c/blob/main/examples/rbtree/rbtree.c)             |

## TODO
- test arena, list and priority queue. test rbtree by case.
- vector + string (as dynamic array)
