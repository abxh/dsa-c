/**
 * @file container_of.h
 * @brief container_of definition
 *
 * For more info, see:
 * @li https://radek.io/posts/magical-container_of-macro/
 * @li https://github.com/clibs/container_of
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @def container_of(ptr, type, member)
 * @brief Obtain a pointer to the struct that contains the member
 *
 * @param[in] ptr               Pointer to the member.
 * @param[in] type              Container type
 * @param[in] member            Member name.
 *
 * @return                      A pointer to the struct containing the member.
 */
#ifndef container_of
#define container_of(ptr, type, member)                    \
    __extension__({                                        \
        const typeof(((type *)0)->member) *__mptr = (ptr); \
        (type *)((char *)__mptr - offsetof(type, member)); \
    })
#endif

#ifdef __cplusplus
}
#endif
