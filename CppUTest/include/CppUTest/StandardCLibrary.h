
/* Must include this first to ensure the StandardC include in CppUTestConfig still happens at the right moment */
#include "CppUTestConfig.h"

#ifndef STANDARDCLIBRARY_H_
#define STANDARDCLIBRARY_H_

#include <stddef.h> /* Needed for size_t */
#include <stdlib.h> /* Needed for malloc */
#include <stdarg.h> /* Needed for ... */
#include <limits.h> /* Needed for some detection of long long and 64 bit */

/* Sometimes the C++ library does an #undef in stdlib of malloc and free. We want to prevent that */
#ifdef __cplusplus
  #include <cstdlib>
  #include <string>
#endif

/* Needed for std::nullptr */
#ifdef __cplusplus
  #include <cstddef>
#endif

/* Kludge to get a va_copy in VC++ V6 and in GCC 98 */
#ifndef va_copy
#ifdef __GNUC__
#define va_copy __va_copy
#else
#define va_copy(copy, original) copy = original;
#endif
#endif

/* Needed to ensure that string.h is included prior to strdup redefinition */
#include <string.h>

#endif
