// ppc_power7.cpp - written and placed in the public domain by
//                  Jeffrey Walton, Uri Blumenthal and Marcel Raad.
//
//    This source file uses intrinsics and built-ins to gain access to
//    Power7 instructions. A separate source file is needed because
//    additional CXXFLAGS are required to enable the appropriate
//    instructions sets in some build configurations.

#include "pch.h"
#include "config.h"

#ifdef CRYPTOPP_GNU_STYLE_INLINE_ASSEMBLY
# include <signal.h>
# include <setjmp.h>
#endif

#if defined(_ARCH_PWR7)
# include "ppc_simd.h"
#endif

// Squash MS LNK4221 and libtool warnings
extern const char PPC_POWER7_FNAME[] = __FILE__;

NAMESPACE_BEGIN(CryptoPP)

// ************************* Feature Probes ************************* //

#ifdef CRYPTOPP_GNU_STYLE_INLINE_ASSEMBLY
extern "C" {
    typedef void (*SigHandler)(int);

    static jmp_buf s_jmpSIGILL;
    static void SigIllHandler(int)
    {
        longjmp(s_jmpSIGILL, 1);
    }
}
#endif  // CRYPTOPP_MS_STYLE_INLINE_ASSEMBLY

#if (CRYPTOPP_BOOL_PPC32 || CRYPTOPP_BOOL_PPC64)
bool CPU_ProbePower7()
{
#if defined(CRYPTOPP_NO_CPU_FEATURE_PROBES)
    return false;
#elif (_ARCH_PWR7) && defined(CRYPTOPP_POWER7_AVAILABLE)
# if defined(CRYPTOPP_GNU_STYLE_INLINE_ASSEMBLY)

    // longjmp and clobber warnings. Volatile is required.
    // http://github.com/weidai11/cryptopp/issues/24 and http://stackoverflow.com/q/7721854
    volatile int result = false;

    volatile SigHandler oldHandler = signal(SIGILL, SigIllHandler);
    if (oldHandler == SIG_ERR)
        return false;

    volatile sigset_t oldMask;
    if (sigprocmask(0, NULLPTR, (sigset_t*)&oldMask))
        return false;

    if (setjmp(s_jmpSIGILL))
        result = false;
    else
    {
        // POWER7 added unaligned loads and store operations
        byte b1[19] = {255, 255, 255, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}, b2[17];

        #if defined(_ARCH_PWR7) && defined(__xlC__)
            // Workaround XL C++ bug
            vec_xst(vec_xl(0, (unsigned int*)(b1+3)), 0, (__vector unsigned int*)(b2+1));
        #elif defined(_ARCH_PWR7)
            vec_xst(vec_xl(0, (__vector unsigned int*)(b1+3)), 0, (__vector unsigned int*)(b2+1));
        #else
            std::memset(b2, 0x00, sizeof(b2));
        #endif

        result = (0 == std::memcmp(b1+3, b2+1, 16));
    }

    sigprocmask(SIG_SETMASK, (sigset_t*)&oldMask, NULLPTR);
    signal(SIGILL, oldHandler);
    return result;
# endif
#else
    return false;
#endif  // _ARCH_PWR7
}

#endif  // PPC32 or PPC64

NAMESPACE_END
