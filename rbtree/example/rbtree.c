#include "rbtree.h"

#define NAME                       rbtree
#define KEY_TYPE                   int
#define KEY_IS_STRICTLY_LESS(a, b) ((a) < (b))
#define FUNCTION_DEFINITIONS
#include "rbtree_template.h"
