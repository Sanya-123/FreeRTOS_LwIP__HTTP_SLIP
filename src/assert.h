#ifndef _ASSERT_H
#define _ASSERT_H

#undef ASSERT

#include "xprintf.h"

// The ASSERT() macro tests the given expression and if it is false, the calling process is
// terminated. A diagnostic message is written to xprintf() and infinity loop is called.
// If expression is true, the ASSERT() macro does nothing. The ASSERT() macro may be removed
// at compile time by defining NDEBUG as a macro (e.g., by using the compiler option -DNDEBUG).

inline static void _assert(const char *function, const char *file, int line, const char *expression)
{
    if (function)
    {
        xprintf("\nAssertion failed: (%s), function: %s, file: %s, line: %d.",
                expression, function, file, line);
    }
    else
    {
        xprintf("\nAssertion failed: (%s), file %s, line %d.", expression, file, line);
    }

    for (;;); // Infinity loop
}

#if defined(NDEBUG)
    #define ASSERT(e) ((void)0)
#else
    #define ASSERT(e) ((e) ? (void)0 :  _assert(__FUNCTION__, __FILE__, __LINE__, #e))
#endif

#endif // _ASSERT_H
