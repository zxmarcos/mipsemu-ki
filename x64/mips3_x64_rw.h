#ifndef MIPS3_X64_RW
#define MIPS3_X64_RW


#include "../mips3.h"
#include "mips3_x64_defs.h"
#include "mips3_x64.h"
#include "xbyak/xbyak.h"
#include "../memory.h"


namespace mips
{

#define GET_EADDR_IN_RDX() \
    do {    \
    auto eaddr = ptr[rbp-8];\
    mov(rdi, (size_t) m_core);\
    mov(rsi, (int64_t)(int32_t)SIMM);\
    add(rsi, RS_x);\
    lea(rdx, eaddr);\
    mov(rax, F_ADR(mips3::translate));\
    call(rax);\
    mov(rdx, eaddr);\
} while (0)

bool mips3_x64::LUI(uint32_t opcode)
{
    if (RTNUM) {
        int64_t value = (int32_t)(IMM << 16);
        mov(RT_q, value);
    }
    return false;
}

bool mips3_x64::SB(uint32_t opcode)
{
    GET_EADDR_IN_RDX();
    mov(rdi, rdx);
    mov(rsi, RT_x);
    mov(rax, F_ADR(mem::write_byte));
    call(rax);
    return false;
}

bool mips3_x64::SH(uint32_t opcode)
{
    GET_EADDR_IN_RDX();
    mov(rdi, rdx);
    mov(rsi, RT_x);
    mov(rax, F_ADR(mem::write_half));
    call(rax);
    return false;
}

bool mips3_x64::SW(uint32_t opcode)
{
    GET_EADDR_IN_RDX();
    mov(rdi, rdx);
    mov(rsi, RT_x);
    mov(rax, F_ADR(mem::write_word));
    call(rax);
    return false;
}

bool mips3_x64::SD(uint32_t opcode)
{
    GET_EADDR_IN_RDX();
    mov(rdi, rdx);
    mov(rsi, RT_x);
    mov(rax, F_ADR(mem::write_dword));
    call(rax);
    return false;
}

// TODO: FIX IT
bool mips3_x64::SDL(uint32_t opcode)
{
    return false;
}

// TODO: FIX IT
bool mips3_x64::SDR(uint32_t opcode)
{
    return false;
}

bool mips3_x64::LWL(uint32_t opcode)
{
    return false;
}

bool mips3_x64::LWR(uint32_t opcode)
{
    return false;
}


// Válido apenas para little endian.
bool mips3_x64::LDL(uint32_t opcode)
{
    return false;
}

// Válido apenas para little endian.
bool mips3_x64::LDR(uint32_t opcode)
{
    return false;
}


bool mips3_x64::LW(uint32_t opcode)
{
    if (RTNUM) {
        GET_EADDR_IN_RDX();
        mov(rdi, rdx);
        mov(rax, F_ADR(mem::read_word));
        call(rax);
        cdqe();
        mov(RT_x, rax);
    }
    return false;
}

bool mips3_x64::LWU(uint32_t opcode)
{
    if (RTNUM) {
        GET_EADDR_IN_RDX();
        mov(rdi, rdx);
        mov(rax, F_ADR(mem::read_word));
        call(rax);
        mov(RT_x, rax);
    }
    return false;
}

bool mips3_x64::LD(uint32_t opcode)
{
    if (RTNUM) {
        GET_EADDR_IN_RDX();
        mov(rdi, rdx);
        mov(rax, F_ADR(mem::read_dword));
        call(rax);
        mov(RT_x, rax);
    }
    return false;
}

bool mips3_x64::LL(uint32_t opcode)
{
    return false;
}

// TODO: FIX IT
bool mips3_x64::LWC1(uint32_t opcode)
{
    return false;
}

// TODO: FIX IT
bool mips3_x64::SWC1(uint32_t opcode)
{
    return false;
}

bool mips3_x64::LB(uint32_t opcode)
{
    if (RTNUM) {
        GET_EADDR_IN_RDX();
        mov(rdi, rdx);
        mov(rax, F_ADR(mem::read_byte));
        call(rax);
        movsx(eax, al);
        cdqe();
        mov(RT_x, rax);
    }
    return false;
}

bool mips3_x64::LBU(uint32_t opcode)
{
    if (RTNUM) {
        GET_EADDR_IN_RDX();
        mov(rdi, rdx);
        mov(rax, F_ADR(mem::read_byte));
        call(rax);
        movzx(eax, al);
        mov(RT_x, rax);
    }
    return false;
}

bool mips3_x64::LH(uint32_t opcode)
{
    if (RTNUM) {
        GET_EADDR_IN_RDX();
        mov(rdi, rdx);
        mov(rax, F_ADR(mem::read_half));
        call(rax);
        movsx(eax, ax);
        cdqe();
        mov(RT_x, rax);
    }
    return false;
}

bool mips3_x64::LHU(uint32_t opcode)
{
    if (RTNUM) {
        GET_EADDR_IN_RDX();
        mov(rdi, rdx);
        mov(rax, F_ADR(mem::read_half));
        call(rax);
        movzx(eax, ax);
        mov(RT_x, rax);
    }
    return false;
}

}

#endif // MIPS3_X64_RW

