/*
 * Handy debugging miscellany.  Mostly preprocessor nastiness.
 */
#pragma once

#include <cassert>

#ifdef DEBUG
//   Intentional omission of semicolon.
//   Invariant method becomes an inlined no-op method in release mode.
#   define DECLARE_INVARIANT() void invariant() 
#   define DEBUG_BLOCK(x) x
#else
#   define DECLARE_INVARIANT() inline void invariant() { }
#   define DEBUG_BLOCK(x)
#endif


