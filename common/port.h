/* Platform-specific compatibility hacks.
 * The idea is to consolodate all the ifdef hackery into one place so
 * code can be simple elsewhere.
 */

#ifndef PORT_H
#define PORT_H

#if defined(_MSC_VER)
#   define DEPRECATED __declspec(deprecated)
#else
#   define DEPRECATED
#endif

// FIXME: only on 32 bit archs
#if 0
    typedef unsigned __int64 u64;
#else
#endif

#if _MSC_VER
    typedef unsigned __int8 u8;
    typedef unsigned __int16 u16;
    typedef unsigned __int32 u32;
    typedef unsigned __int64 u64;

    typedef signed __int8 s8;
    typedef signed __int16 s16;
    typedef signed __int32 s32;
    typedef signed __int64 s64;
#else // assume GCC
    typedef unsigned char u8;
    typedef unsigned short u16;
    typedef unsigned int u32;
    typedef unsigned long long u64;

    typedef signed char s8;
    typedef signed short s16;
    typedef signed int s32;
    typedef signed long long s64;
#endif

// signed/unsigned ints of whatever size.
typedef size_t uint;
//typedef ptrdiff_t sint;

#if defined(_MSC_VER) && _MSC_VER < 1300
    // Silence stupid not-really-a-warning. (identifier too long for debugger hurk blah blah etc)
#   pragma warning (disable:4786)
    // Fix broken for() scoping in VC6
#   define for if (0); else for
#endif

// Calling convention poocrap.  Needed in a handful of places.
#ifdef WIN32
#   define IKA_STDCALL __stdcall
#else
#   define IKA_STDCALL
#endif

#endif
