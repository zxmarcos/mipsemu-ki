#ifndef MIPS3_X64_COP1
#define MIPS3_X64_COP1


#include "../mips3.h"
#include "mips3_x64_defs.h"
#include "mips3_x64.h"
#include "xbyak/xbyak.h"

namespace mips
{

#if 0
void mips3::cop1_execute_32(uint32_t opcode)
{
    //qDebug() << QString(dasm(opcode, m_prev_pc).c_str());
    switch (RSNUM) {
    // MFC1 rt, rd
    case 0x00:
        if (RTNUM)
            RT = (int32_t) m_state.cpr[1][RDNUM];
        break;

    // DMFC1 rt, rd
    case 0x01:
        if (RTNUM)
            RT = (uint64_t) m_state.cpr[1][RDNUM];
        break;

    // CFC1 rt, fs
    case 0x02:
        if (RTNUM)
            RT = (int32_t) m_state.fcr[FSNUM];
        break;

    // MTC1 rt, fs
    case 0x04:
        m_state.cpr[1][FSNUM] = (uint32_t) RT;
        break;

    // DMTC1 rt, fs
    case 0x05:
        m_state.cpr[1][FSNUM] = RT;
        break;

    // CTC1 rt, fs
    case 0x06:
        m_state.fcr[FSNUM] = (int32_t) RT;
        break;

    // BC
    case 0x08:
    {
        switch ((opcode >> 16) & 3) {
        // BC1F offset
        case 0x00:
            if (~FCR31 & 0x800000) {
                m_next_pc = m_state.pc + ((int32_t)(SIMM) << 2);
                m_delay_slot = true;
            }
            break;

        // BC1FL offset
        case 0x02:
            if (~FCR31 & 0x800000) {
                m_next_pc = m_state.pc + ((int32_t)(SIMM) << 2);
                m_delay_slot = true;
            } else
                m_state.pc += 4;
            break;

        // BC1T offset
        case 0x01:
            if (FCR31 & 0x800000) {
                m_next_pc = m_state.pc + ((int32_t)(SIMM) << 2);
                m_delay_slot = true;
            }
            break;

        // BC1TL offset
        case 0x03:
            if (FCR31 & 0x800000) {
                m_next_pc = m_state.pc + ((int32_t)(SIMM) << 2);
                m_delay_slot = true;
            } else
                m_state.pc += 4;
            break;

        default:
            qDebug() << QString::number(m_state.pc, 16) << "OpBC:" << ((opcode >> 16) & 0x1F) << "[FPU32][COP]";
            break;
        }
        break;
    }

    default:
    {
        switch (opcode & 0x3F) {

        // ADD.fmt
        case 0x00:
            if (SP) FD_s = FS_s + FT_s;
            else    FD_d = FS_d + FT_d;
            break;

        // SUB.fmt
        case 0x01:
            if (SP) FD_s = FS_s - FT_s;
            else    FD_d = FS_d - FT_d;
            break;

        // MUL.fmt
        case 0x02:
            if (SP) FD_s = FS_s * FT_s;
            else    FD_d = FS_d * FT_d;
            break;

        // DIV.fmt
        case 0x03:
            if (SP) FD_s = FS_s / FT_s;
            else    FD_d = FS_d / FT_d;
            break;

        // SQRT.fmt
        case 0x04:
            if (SP) FD_s = sqrt(FS_s);
            else    FD_d = sqrt(FS_d);
            break;

        // ABS.fmt
        case 0x05:
            if (SP) FD_s = abs(FS_s);
            else    FD_d = abs(FS_d);
            break;

        // MOV.fmt
        case 0x06:
            if (SP) FD_s = FS_s;
            else    FD_d = FS_d;
            break;

        // NEG.fmt
        case 0x07:
            if (SP) FD_s = -FS_s;
            else    FD_d = -FS_d;
            break;

        // CVT.S.x
        case 0x20:
            if (INTEGER) {
                if (SP) FD_s = (int32_t) FS_w;
                else    FD_s = (int64_t) FS_l;
            } else
                FD_s = FS_d;
            break;

        // CVT.W.x
        case 0x24:
            if (SP) FD_w = (int32_t) FS_s;
            else    FD_w = (int32_t) FS_d;
            break;

        // C.UN.x fs, ft
        case 0x32:
            if (SP) FCR31 = (isnan(FS_s) || isnan(FT_s)) ? FCR31 | 0x800000 : FCR31 & ~0x800000;
            else    FCR31 &= ~0x800000;
            break;

        // C.OLT.x fs, ft
        case 0x34:
            if (SP) FCR31 = (FS_s < FT_s) ? FCR31 | 0x800000 : FCR31 & ~0x800000;
            else    FCR31 = (FS_d < FT_d) ? FCR31 | 0x800000 : FCR31 & ~0x800000;
            break;

        // C.F fs, ft
        case 0x3C:
            FCR31 &= ~800000;
            break;

        default:
            qDebug() << QString::number(m_state.pc, 16) << "Op:" << RSNUM << (opcode & 0x3F) << "[FPU32]";
            break;
        }
    }
    }
}
#endif

bool mips3_x64::BC1F(uint32_t opcode)
{
    if (m_is_delay_slot)
        return true;

    addr_t nextpc = m_drc_pc + ((int32_t)(SIMM) << 2);
    addr_t eaddr = 0;

    m_core->translate(m_drc_pc, &eaddr);
    uint32_t next_opcode = mem::read_word(eaddr);

    m_drc_pc += 4;

    update_icounter();

    inLocalLabel();
    mov(rcx, FCR_ref(31));
    mov(rcx, ptr[rcx]);
    not(ecx);
    and(ecx, 0x800000);
    test(ecx, ecx);
    je(".false");
    {
        m_is_delay_slot = true;
        compile_instruction(next_opcode);
        m_is_delay_slot = false;
        jmp_to_block(nextpc);
    }

    L(".false");
    compile_instruction(next_opcode);
    outLocalLabel();
    check_icounter();
    return false;

}

bool mips3_x64::BC1FL(uint32_t opcode)
{
    if (m_is_delay_slot)
        return true;

    addr_t nextpc = m_drc_pc + ((int32_t)(SIMM) << 2);
    addr_t eaddr = 0;

    m_core->translate(m_drc_pc, &eaddr);
    uint32_t next_opcode = mem::read_word(eaddr);

    m_drc_pc += 4;

    update_icounter();

    inLocalLabel();
    mov(rcx, FCR_ref(31));
    mov(rcx, ptr[rcx]);
    not(ecx);
    and(ecx, 0x800000);
    test(ecx, ecx);
    je(".false");
    {
        m_is_delay_slot = true;
        compile_instruction(next_opcode);
        m_is_delay_slot = false;
        jmp_to_block(nextpc);
    }

    L(".false");
    outLocalLabel();
    check_icounter();
    return false;

    return false;
}

bool mips3_x64::BC1T(uint32_t opcode)
{
    if (m_is_delay_slot)
        return true;

    addr_t nextpc = m_drc_pc + ((int32_t)(SIMM) << 2);
    addr_t eaddr = 0;

    m_core->translate(m_drc_pc, &eaddr);
    uint32_t next_opcode = mem::read_word(eaddr);

    m_drc_pc += 4;

    update_icounter();

    inLocalLabel();
    mov(rcx, FCR_ref(31));
    mov(rcx, ptr[rcx]);
    and(ecx, 0x800000);
    test(ecx, ecx);
    je(".false");
    {
        m_is_delay_slot = true;
        compile_instruction(next_opcode);
        m_is_delay_slot = false;
        jmp_to_block(nextpc);
    }

    L(".false");
    compile_instruction(next_opcode);
    outLocalLabel();
    check_icounter();
    return false;

}

bool mips3_x64::BC1TL(uint32_t opcode)
{
    if (m_is_delay_slot)
        return true;

    addr_t nextpc = m_drc_pc + ((int32_t)(SIMM) << 2);
    addr_t eaddr = 0;

    m_core->translate(m_drc_pc, &eaddr);
    uint32_t next_opcode = mem::read_word(eaddr);

    m_drc_pc += 4;

    update_icounter();

    inLocalLabel();
    mov(rcx, FCR_ref(31));
    mov(rcx, ptr[rcx]);
    and(ecx, 0x800000);
    test(ecx, ecx);
    je(".false");
    {
        m_is_delay_slot = true;
        compile_instruction(next_opcode);
        m_is_delay_slot = false;
        jmp_to_block(nextpc);
    }

    L(".false");
    outLocalLabel();
    check_icounter();
    return false;
}

bool mips3_x64::compile_cop1(uint32_t opcode)
{
    bool result = false;

    switch (RSNUM) {
    // MFC1 rt, rd
    case 0x00:
        if (RTNUM) {
            mov(rax, FPR_ref(RDNUM));
            mov(rcx, RT_ref);
            mov(eax, ptr[rax]);
            cdqe();
            mov(ptr[rcx], rax);
        }
        break;

    // DMFC1 rt, rd
    case 0x01:
        if (RTNUM) {
            mov(rax, FPR_ref(RDNUM));
            mov(rcx, RT_ref);
            mov(rax, ptr[rax]);
            mov(ptr[rcx], rax);
        }
        break;

    // CFC1 rt, fs
    case 0x02:
        if (RTNUM) {
            mov(rax, FCR_ref(FSNUM));
            mov(rcx, RT_ref);
            mov(eax, ptr[rax]);
            cdqe();
            mov(ptr[rcx], rax);
        }
        break;

    // MTC1 rt, fs
    case 0x04:
        mov(rax, FPR_ref(FSNUM));
        mov(rcx, RT_ref);
        mov(ecx, ptr[rcx]);
        mov(ptr[rax], rcx);
        break;

    // DMTC1 rt, fs
    case 0x05:
        mov(rax, FPR_ref(FSNUM));
        mov(rcx, RT_ref);
        mov(rcx, ptr[rcx]);
        mov(ptr[rax], rcx);
        break;

    // CTC1 rt, fs
    case 0x06:
        mov(rcx, FCR_ref(FSNUM));
        mov(rax, RT_ref);
        mov(eax, ptr[rax]);
        cdqe();
        mov(ptr[rcx], rax);
        break;

    // BC
    case 0x08:
    {
        switch ((opcode >> 16) & 3) {
        // BC1F offset
        case 0x00: result = BC1F(opcode); break;

        // BC1FL offset
        case 0x02: result = BC1FL(opcode); break;

        // BC1T offset
        case 0x01: result = BC1T(opcode); break;
            break;

        // BC1TL offset
        case 0x03: result = BC1TL(opcode); break;
        }
        break;
    }

    default:
        fallback(opcode, &mips3::cop1_execute_32);
        break;
    }
    return result;
}

}

#endif // MIPS3_X64_COP1

