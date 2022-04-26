#ifndef __nnc_arch_HPP__
#define __nnc_arch_HPP__

#if defined(__x86_64__) || defined(_M_X64)
#define NNC_ARCH_X86_64 1
#else
#error Could not determine architecture
#endif

#endif
