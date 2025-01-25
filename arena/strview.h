#include <assert.h>
#include <stddef.h>
#include <string.h>

struct strview {
    size_t len;
    const char *str;
};

void strview_lit(struct strview *self, const char *str)
{
    self->len = strlen(str);
    self->str = str;
}

void strview_lit_w_len(struct strview *self, const char *str, size_t len)
{
    assert(strlen(str) < len);
    self->len = len;
    self->str = str;
}

