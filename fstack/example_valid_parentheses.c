#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define NAME       paren_stk
#define VALUE_TYPE char
#define TYPE_DEFINITIONS
#define FUNCTION_DEFINITIONS
#define FUNCTION_LINKAGE static inline
#include "fstack_template.h"

void print_paren_stk(struct paren_stk *stk)
{
    if (stk->count > 0) {
        printf("%c", paren_stk_at(stk, 0));
    }
    else {
        printf("(empty)");
    }
    for (size_t i = 1; i < stk->count; i++) {
        printf(" <- %c", paren_stk_at(stk, (uint32_t)i));
    }
    printf("\n");
}

char matching_paren(char c)
{
    switch (c) {
    case '(':
        return ')';
    case '{':
        return '}';
    case '[':
        return ']';
    case ')':
        return '(';
    case '}':
        return '{';
    case ']':
        return '[';
    }
    return '\0';
}

bool isValid(char *s)
{
    const size_t n = strlen(s);
    struct paren_stk *stk = paren_stk_create((uint32_t)n);

    bool retVal = true;

    for (size_t i = 0; i < n; i++) {
        switch (s[i]) {
        case '(':
        case '{':
        case '[':
            if (!paren_stk_is_full(stk)) {
                paren_stk_push(stk, matching_paren(s[i]));
            }
            else {
                assert(false);
            }
            break;
        case ')':
        case '}':
        case ']':
            retVal &= !paren_stk_is_empty(stk) && paren_stk_pop(stk) == s[i];
            break;
        default:
            assert(false);
            break;
        }
    }
    retVal &= stk->count == 0;

    paren_stk_destroy(stk);

    return retVal;
}

#define print_and_assert(expr, expected)                       \
    __extension__({                                            \
        bool res = (expr);                                     \
        assert(res == (expected));                             \
        printf("%-17s = %s\n", #expr, res ? "true" : "false"); \
    })

int main(void)
{
    (void)(print_paren_stk);

    print_and_assert(isValid("()"), true);
    print_and_assert(isValid("("), false);
    print_and_assert(isValid(")"), false);
    print_and_assert(isValid("(]"), false);
    print_and_assert(isValid("([]){}"), true);
    print_and_assert(isValid("([){}"), false);
    print_and_assert(isValid("{[][]}"), true);
    print_and_assert(isValid("{[[]}"), false);
    print_and_assert(isValid("{[(])}"), false);
}
