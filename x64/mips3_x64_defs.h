#ifndef MIPS3_X64_DEFS
#define MIPS3_X64_DEFS

#include "../mipsdef.h"

#define DEBUG_DRC   1

#if DEBUG_DRC
# define drc_log(...)   printf("drc: " __VA_ARGS__); fflush(stdout)
#else
# define drc_log(...)
#endif

#define DEBUG_CALL(f)   \
    mov(rax, (size_t)(void*)&f);  \
    call(rax);

#define CORE_PC   (m_core->m_state.pc)
#define RS_ref    ((size_t)&m_core->m_state.r[RSNUM])
#define RD_ref    ((size_t)&m_core->m_state.r[RDNUM])
#define RT_ref    ((size_t)&m_core->m_state.r[RTNUM])
#define LO_ref    ((size_t)&m_core->m_state.lo)
#define HI_ref    ((size_t)&m_core->m_state.hi)
#define R_ref(n)  ((size_t)&m_core->m_state.r[n])
#define ADR(n)    ((size_t)&n)
#define F_ADR(f)  ((size_t)(void*)&f)

#define FPR_ref(n)  ((size_t)&m_core->m_state.cpr[1][n])
#define FCR_ref(n)  ((size_t)&m_core->m_state.fcr[n])

#define Rn_x(n) ptr[rbx+((size_t)offsetof(mips3::cpu_state, r[n]))]
#define RS_x    ptr[rbx+((size_t)offsetof(mips3::cpu_state, r[RSNUM]))]
#define RD_x    ptr[rbx+((size_t)offsetof(mips3::cpu_state, r[RDNUM]))]
#define RT_x    ptr[rbx+((size_t)offsetof(mips3::cpu_state, r[RTNUM]))]
#define LO_x    ptr[rbx+((size_t)offsetof(mips3::cpu_state, lo))]
#define HI_x    ptr[rbx+((size_t)offsetof(mips3::cpu_state, hi))]

#define RS_q    qword[rbx+((size_t)offsetof(mips3::cpu_state, r[RSNUM]))]
#define RD_q    qword[rbx+((size_t)offsetof(mips3::cpu_state, r[RDNUM]))]
#define RT_q    qword[rbx+((size_t)offsetof(mips3::cpu_state, r[RTNUM]))]

#endif // MIPS3_X64_DEFS

