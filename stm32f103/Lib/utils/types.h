#ifndef __LIB_UTILS_TYPES_H__
#define __LIB_UTILS_TYPES_H__

/* Get bool, true and false.
 * If using GCC then get stdbool, otherwise create definitions here,
 */
#if (defined(__GNUC__) || defined(C99)) && !defined(_lint)
#include <stdbool.h>
typedef bool bool_t;
#elif defined(__cplusplus)
/* bool is already defined in C++ */
#else
typedef _Bool bool_t;
#define true ((bool_t) 1)
#define false ((bool_t) 0)

typedef bool_t bool;
#endif

// Use meaningful names when bool used for success/failure, e.g. for function returns.
#define SUCCESS true
#define FAILURE false

#if defined(__GNUC__) || defined(C99)
#include <stdint.h>
#include <stddef.h>
#else
typedef signed char int8_t;
typedef short       int16_t;
typedef int         int32_t;
typedef long long   int64_t;

typedef unsigned char      uint8_t;
typedef unsigned short     uint16_t;
typedef unsigned int       uint32_t;
typedef unsigned long long uint64_t;

#ifndef NULL
#define NULL ((void*)0)
#endif
#endif

#define MAX_UINT8  0xFF
#define MAX_UINT16 0xFFFF
#define MAX_INT32  0x7FFFFFFF
#define MAX_UINT32 0xFFFFFFFF
#define MAX_UINT64 0xFFFFFFFFFFFFFFFFULL
#define MIN_INT16  (-0x8000)
#define MAX_INT16  (0x7FFF)

typedef uint32_t iot_addrword_t;
typedef uint32_t iot_addr_t;
#endif /*__LIB_UTILS_TYPES_H__*/
