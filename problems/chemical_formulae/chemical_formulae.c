#include <stdbool.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "strmap.h"

#include <stdio.h>
#include <stdlib.h>

#define max(x, y) ((x) > (y) ? (x) : (y))

#define line_len_max 150
static uint64_t result[line_len_max][line_len_max];

uint64_t longest_common_subsequence(const char* str1, size_t n, const char* str2, size_t m) {
    // https://en.wikibooks.org/wiki/Algorithm_Implementation/Strings/Longest_common_subsequence
    memset(result, 0, sizeof(result));
    for (size_t i = 1; i <= n; i++) {
        for (size_t j = 1; j <= m; j++) {
            if (tolower(str1[i - 1]) == tolower(str2[j - 1])) {
                result[i][j] = result[i - 1][j - 1] + 1;
            } else {
                result[i][j] = max(result[i - 1][j], result[i][j - 1]);
            }
        }
    }
    return result[n][m];
}

#define abs_diff(x, y) ((x) >= (y) ? (x) - (y) : (y) - (x))

static char* CMP_CUSTOM_USR_INP_P = NULL;
int cmp_custom(const void* s1, const void* s2) {
    size_t n = strlen(CMP_CUSTOM_USR_INP_P);
    size_t m1 = strlen(*(char**)s1);
    size_t m2 = strlen(*(char**)s2);

    uint64_t lcs1 = longest_common_subsequence(CMP_CUSTOM_USR_INP_P, n, *(char**)s1, m1);
    uint64_t lcs2 = longest_common_subsequence(CMP_CUSTOM_USR_INP_P, n, *(char**)s2, m2);

    if (lcs1 != lcs2) {
        return -(lcs1 < lcs2) + (lcs1 > lcs2);
    }

    size_t diff1 = abs_diff(m1, n);
    size_t diff2 = abs_diff(m2, n);

    return (diff2 > diff1) - (diff2 < diff1);
}

int main(void) {

    // prepare stuff:

    // printf("Please wait...\n");
    // int status = system("python get_data.py");
    // if (status != 0) {
    //     return 1;
    // }

    FILE* fp = fopen("data.csv", "r");
    if (fp == NULL) {
        return 1;
    }
    StrMap* strmap_p;
    if (!strmap_init(&strmap_p)) {
        return 1;
    }

    // read from file and store data as keys and values in strmap:
    size_t lines = 0;
    while (true) {
        if (lines == 0) {
            lines++;
            continue;
        }
        char buf[line_len_max];
        if (fgets(buf, line_len_max, fp) == NULL) {
            break;
        }
        size_t n = strnlen(buf, 150);
        buf[n - 1] = '\0'; // remove newline

        size_t i = strcspn(buf, ",");
        buf[i] = '\0';

        size_t j;
        if (buf[i + 1] != '\"') {
            j = i + 1 + strcspn(&buf[i + 1], ",");
            buf[j] = '\0';
        } else {
            j = i + 2 + strcspn(&buf[i + 2], "\"") + 1;
            buf[j - 1] = '\0';
            buf[j] = '\0';
            i += 1;
        }

        char* formula_str = &buf[0];
        char* synonym_str = &buf[i + 1];
        char* cas_str = &buf[j + 1];

        char* iter_str = synonym_str;
        while (*iter_str++ != '\0') {
            *iter_str = tolower(*iter_str);
        }

        char buf1[line_len_max * 2];
        sprintf(buf1, "%s%s%s", formula_str, *cas_str != '\0' ? ", CAS: " : "", cas_str);
        strmap_set(strmap_p,
                   synonym_str, // key
                   buf1         // value
        );

        lines++;
    }

    // create an array of key references:
    char** keys_arr_pp = malloc(sizeof(char*) * lines);
    {
        size_t keys_arr_index = 0;

        // required for iterating strmap:
        size_t list_index;
        StrMapNode* next_p;
        char* key_p;
        char* value_p;

        strmap_for_each(strmap_p, list_index, next_p, key_p, value_p) {
            keys_arr_pp[keys_arr_index++] = key_p; // store reference
        }

        lines = keys_arr_index; // because there are some overlapping keys for some reason. TODO: fix
    }
    // printf("Type your input:\n");

    // actual program:
    char* line_p = NULL;
    size_t n = 0;
    ssize_t len = 0;
    while (0 < (len = getline(&line_p, &n, stdin))) {
        CMP_CUSTOM_USR_INP_P = &line_p[0];
        qsort(keys_arr_pp, lines, sizeof(char*), cmp_custom);
        for (size_t i = lines - 1; i >= lines - 5; i--) {
            printf(" -> %s (%s)\n", keys_arr_pp[i], strmap_get(strmap_p, keys_arr_pp[i]));
        }
    }
    // clean up stuff:
    free(line_p);
    free(keys_arr_pp);
    strmap_deinit(&strmap_p);
    fclose(fp);

    return 0;
}
