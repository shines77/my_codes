
#ifndef KERNEL_HDE_H
#define KERNEL_HDE_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#if defined(_WIN64) || defined(_M_AMD64) || defined(_M_IA64) || \
    defined(_AMD64_) || defined(_IA64_) || defined(_ARM_) || defined(_ARM64_)

#ifndef hde_status
#define hde_status      hde64s
#endif

#include "hde/hde64.h"

#else // !IS_X64_DRIVER

#ifndef hde_status
#define hde_status      hde32s
#endif

#include "hde/hde32.h"

#endif // IS_X64_DRIVER(_AMD64_ | _IA64_ | _ARM_ | _ARM64_)

EXTERN_C_START

/////////////////////////////////////////////////////////

static inline
/* __cdecl */
unsigned int hde_disasm(const void * code, hde_status * hs)
{
#if defined(_WIN64) || defined(_M_AMD64) || defined(_M_IA64) || \
    defined(_AMD64_) || defined(_IA64_) || defined(_ARM_) || defined(_ARM64_)
    return hde64_disasm(code, hs);
#else
    return hde32_disasm(code, hs);
#endif // IS_X64_DRIVER(_AMD64_ | _IA64_ | _ARM_ | _ARM64_)
}

/////////////////////////////////////////////////////////

EXTERN_C_END

#endif // KERNEL_HDE_H
