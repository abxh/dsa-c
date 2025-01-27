#pragma once

#include <string.h>
#include "fnvhash.h"

#define NAME               strint_ht
#define KEY_TYPE           char *
#define VALUE_TYPE         int
#define KEY_IS_EQUAL(a, b) (strcmp((a), (b)) == 0)
#define HASH_FUNCTION(key) (fnvhash_32_str(key))
#define TYPE_DEFINITIONS
#include "fhashtable_template.h"
