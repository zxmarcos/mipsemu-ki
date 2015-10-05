// ============================================================================
// MIP3 JIT for AMD64
//
// Marcos Medeiros
// ============================================================================
#include <cstdio>
#include <QDebug>
#include "mips3_x64.h"
#include "xbyak/xbyak.h"
#include "../mips3.h"
#include "../mipsdef.h"
#include "../memory.h"
#include "mips3_x64_defs.h"
#include "mips3_x64_rw.h"
#include "mips3_x64_branch.h"
#include "mips3_x64_bitops.h"
#include "mips3_x64_arithm.h"
#include "mips3_x64_shift.h"
#include "mips3_x64_misc.h"
#include "mips3_x64_cop1.h"
#include "udis86/udis86.h"

#define LOG_DYNAREC         0
#define LOG_DYNAREC_DASM    0
#define FULL_FALLBACK       0

namespace mips
{

mips3_x64::mips3_x64(mips3 *interpreter) : CodeGenerator(1024 * 1024 * 32)
{
    m_core = interpreter;

    ud_init(&m_udobj);
    ud_set_mode(&m_udobj, 64);
    ud_set_syntax(&m_udobj, UD_SYN_INTEL);

    qDebug() << "STATE:" << QString::number((size_t)&m_core->m_state, 16);
}

void mips3_x64::run(int cycles)
{
    m_icounter = cycles;
    m_stop_translation = false;
    m_translate_failed = false;

    void *recompiled_code;
    while (m_icounter > 0) {
        recompiled_code = get_block(m_core->m_state.pc);

        if (recompiled_code == nullptr) {
            auto ptr = compile_block(m_core->m_state.pc);
            if (m_translate_failed)
                break;

            m_blocks[m_core->m_state.pc] = ptr;
            recompiled_code = (ptr);
        }

#if 0
        else
            qDebug() << "Block FOUND" << m_blocks[m_core->m_state.pc] << "for"
                     << QString::number(m_core->m_state.pc, 16);
#endif
        Xbyak::CastTo<void(*)()>(recompiled_code)();
    }
    if (m_translate_failed)
        qDebug() << "Translation failed at PC:" << QString::number(m_drc_pc, 16);
    //qDebug() << "Cycles:" << m_icounter;
}



void mips3_x64::prolog()
{
    push(rbp);
    push(rbx);
    mov(rbp, rsp);
    sub(rsp, 16);
    mov(rbx, ADR(m_core->m_state));
    check_icounter();
}

void mips3_x64::epilog(bool do_ret)
{
    add(rsp, 16);
    pop(rbx);
    pop(rbp);
    if (do_ret)
        ret();
}



void *mips3_x64::compile_block(addr_t pc)
{
    static int depth = 0;
    m_drc_pc = pc;

    depth++;

#if LOG_DYNAREC
    qDebug() << "Recompile block at" << QString::number(m_drc_pc, 16);
#endif

    uint32_t opcode;
    addr_t eaddr;
    bool do_recompile = true;

    void *block_ptr = Xbyak::CastTo<void*>(getCurr());

#if LOG_DYNAREC
    qDebug() << "BlockPtr:" << QString::number((uint64_t)block_ptr, 16);
#endif


    prolog();

    m_block_icounter = 0;

    while (do_recompile) {
        m_core->translate(m_drc_pc, &eaddr);
        opcode = mem::read_word(eaddr);
        m_drc_pc += 4;
        m_block_icounter++;
        if (compile_instruction(opcode)) {
            // Jump/Branch Instr
            do_recompile = false;
        }
    }

    ready();

    depth--;
#if LOG_DYNAREC
    if (!depth) {
        qDebug() << "=======================================";
        qDebug() << "Generated code for" << QString::number(pc, 16);
        qDebug() << "=======================================";
        // show recompiled code
        ud_set_input_buffer(&m_udobj, (unsigned char *) block_ptr,
                            (Xbyak::CastTo<uint8_t*>(getCurr()) - Xbyak::CastTo<uint8_t*>(block_ptr)));
        ud_set_pc(&m_udobj, (uint64_t) block_ptr);
        uint64_t dasm_pc = (uint64_t) block_ptr;
        unsigned k;
        while ((k = ud_disassemble(&m_udobj))) {
            qDebug() << QString::number(dasm_pc, 16) << ud_insn_asm(&m_udobj);
            dasm_pc += k;
        }
    }
#endif
    return block_ptr;
}

void *mips3_x64::get_block(addr_t pc)
{
    if (m_blocks.find(pc) == m_blocks.end())
        return nullptr;
    return m_blocks[pc];
}


bool mips3_x64::compile_instruction(uint32_t opcode)
{
#if LOG_DYNAREC_DASM
    qDebug() << QString(m_core->dasm(opcode, m_drc_pc - 4).c_str());
#endif

    bool result = false;
    switch (opcode >> 26) {
    // SPECIAL
    case 0x00:
    {
        switch (opcode & 0x3F) {


#if !FULL_FALLBACK
        case 0x00: result = SLL(opcode); break;
        case 0x02: result = SRL(opcode); break;
        case 0x03: result = SRA(opcode); break;
        case 0x04: result = SLLV(opcode); break;
        case 0x06: result = SRLV(opcode); break;
        case 0x07: result = SRAV(opcode); break;
        case 0x14: result = DSLLV(opcode); break;
        case 0x16: result = DSLRV(opcode); break;
        case 0x38: result = DSLL(opcode); break;
        case 0x3A: result = DSRL(opcode); break;
        case 0x3B: result = DSRA(opcode); break;
        case 0x3C: result = DSLL32(opcode); break;
        case 0x3E: result = DSRL32(opcode); break;
        case 0x3F: result = DSRA32(opcode); break;

        case 0x10: result = MFHI(opcode); break;
        case 0x11: result = MTHI(opcode); break;
        case 0x12: result = MFLO(opcode); break;
        case 0x13: result = MTLO(opcode); break;

//        case 0x18: fallback(opcode, &mips3::MULT); break;
//        case 0x19: fallback(opcode, &mips3::MULTU); break;
//        case 0x1A: fallback(opcode, &mips3::DIV); break;
//        case 0x1B: fallback(opcode, &mips3::DIVU); break;
//        case 0x1C: fallback(opcode, &mips3::DMULT); break;
//        case 0x1E: fallback(opcode, &mips3::DDIV); break;
//        case 0x1F: fallback(opcode, &mips3::DDIVU); break;

        case 0x18: result = MULT(opcode); break;
        case 0x19: result = MULTU(opcode); break;
        case 0x1A: result = DIV(opcode); break;
        case 0x1B: result = DIVU(opcode); break;
        case 0x1C: result = DMULT(opcode); break;
        case 0x1E: result = DDIV(opcode); break;
        case 0x1F: result = DDIVU(opcode); break;

        case 0x20: result = ADD(opcode); break;
        case 0x21: result = ADDU(opcode); break;
        case 0x22: result = SUB(opcode); break;
        case 0x23: result = SUBU(opcode); break;
        case 0x2D: result = DADDU(opcode); break;
        case 0x2F: result = DSUBU(opcode); break;

        case 0x24: result = AND(opcode); break;
        case 0x25: result = OR(opcode); break;
        case 0x26: result = XOR(opcode); break;
        case 0x27: result = NOR(opcode); break;

        case 0x2A: result = SLT(opcode); break;
        case 0x2B: result = SLTU(opcode); break;

#else
        case 0x00: fallback(opcode, &mips3::SLL); break;
        case 0x02: fallback(opcode, &mips3::SRL); break;
        case 0x03: fallback(opcode, &mips3::SRA); break;
        case 0x04: fallback(opcode, &mips3::SLLV); break;
        case 0x06: fallback(opcode, &mips3::SRLV); break;
        case 0x07: fallback(opcode, &mips3::SRAV); break;
        case 0x14: fallback(opcode, &mips3::DSLLV); break;
        case 0x16: fallback(opcode, &mips3::DSLRV); break;
        case 0x38: fallback(opcode, &mips3::DSLL); break;
        case 0x3A: fallback(opcode, &mips3::DSRL); break;
        case 0x3B: fallback(opcode, &mips3::DSRA); break;
        case 0x3C: fallback(opcode, &mips3::DSLL32); break;
        case 0x3E: fallback(opcode, &mips3::DSRL32); break;
        case 0x3F: fallback(opcode, &mips3::DSRA32); break;

        case 0x10: fallback(opcode, &mips3::MFHI); break;
        case 0x11: fallback(opcode, &mips3::MTHI); break;
        case 0x12: fallback(opcode, &mips3::MFLO); break;
        case 0x13: fallback(opcode, &mips3::MTLO); break;

        case 0x18: fallback(opcode, &mips3::MULT); break;
        case 0x19: fallback(opcode, &mips3::MULTU); break;
        case 0x1A: fallback(opcode, &mips3::DIV); break;
        case 0x1B: fallback(opcode, &mips3::DIVU); break;
        case 0x1C: fallback(opcode, &mips3::DMULT); break;
        case 0x1E: fallback(opcode, &mips3::DDIV); break;
        case 0x1F: fallback(opcode, &mips3::DDIVU); break;
        case 0x20: fallback(opcode, &mips3::ADD); break;
        case 0x21: fallback(opcode, &mips3::ADDU); break;
        case 0x22: fallback(opcode, &mips3::SUB); break;
        case 0x23: fallback(opcode, &mips3::SUBU); break;
        case 0x2D: fallback(opcode, &mips3::DADDU); break;
        case 0x2F: fallback(opcode, &mips3::DSUBU); break;

        case 0x24: fallback(opcode, &mips3::AND); break;
        case 0x25: fallback(opcode, &mips3::OR); break;
        case 0x26: fallback(opcode, &mips3::XOR); break;
        case 0x27: fallback(opcode, &mips3::NOR); break;

        case 0x2A: fallback(opcode, &mips3::SLT); break;
        case 0x2B: fallback(opcode, &mips3::SLTU); break;

#endif

        case 0x08: result = JR(opcode); break;
        case 0x09: result = JALR(opcode); break;
        case 0x0C: qDebug() << "syscall"; break;
        case 0x0D: qDebug() << "break"; break;
        default:
            qDebug() << QString::number(m_drc_pc, 16) << QString::number(opcode & 0x3F, 16) << "[special]";
            translate_failed();
            result = true;
            exit(-3);
            break;
        }
        break;
    }

        // REGIMM
    case 0x01:
    {
        switch ((opcode >> 16) & 0x1F) {
        case 0x00: result = BLTZ(opcode); break;
        case 0x01: result = BGEZ(opcode); break;
        case 0x10: result = BLTZAL(opcode); break;
        case 0x11: result = BGEZAL(opcode); break;
        default:
            qDebug() << QString::number(m_drc_pc, 16) << "Op:" << QString::number((opcode >> 16) & 0x1F, 16) << " [regimm]";
            translate_failed();
            result = true;
            exit(-3);
            break;
        }

        break;
    }

    case 0x02: result = J(opcode); break;
    case 0x03: result = JAL(opcode); break;
    case 0x04: result = BEQ(opcode); break;
    case 0x05: result = BNE(opcode); break;
    case 0x06: result = BLEZ(opcode); break;
    case 0x07: result = BGTZ(opcode); break;

#if !FULL_FALLBACK
    case 0x08: result = ADDI(opcode); break;
    case 0x09: result = ADDIU(opcode); break;
    case 0x0A: result = SLTI(opcode); break;
    case 0x0B: result = SLTIU(opcode); break;
    case 0x0C: result = ANDI(opcode); break;
    case 0x0D: result = ORI(opcode); break;
    case 0x0E: result = XORI(opcode); break;
    case 0x0F: result = LUI(opcode); break;
#else
    case 0x08: fallback(opcode, &mips3::ADDI); break;
    case 0x09: fallback(opcode, &mips3::ADDIU); break;
    case 0x0A: fallback(opcode, &mips3::SLTI); break;
    case 0x0B: fallback(opcode, &mips3::SLTIU); break;
    case 0x0C: fallback(opcode, &mips3::ANDI); break;
    case 0x0D: fallback(opcode, &mips3::ORI); break;
    case 0x0E: fallback(opcode, &mips3::XORI); break;
    case 0x0F: fallback(opcode, &mips3::LUI); break;
#endif
    case 0x10:
    {
        fallback(opcode, &mips3::cop0_execute);
        result = false;
        break;
    }
#if !FULL_FALLBACK
    case 0x11: result = compile_cop1(opcode); break;
    case 0x18: result = DADDI(opcode); break;
    case 0x19: result = DADDIU(opcode); break;
#else
    case 0x11: fallback(opcode, &mips3::cop1_execute_32); break;
    case 0x18: fallback(opcode, &mips3::DADDI); break;
    case 0x19: fallback(opcode, &mips3::DADDIU); break;
#endif

    case 0x2F: break;

        // Load & Store fallbacks
//    case 0x23: fallback(opcode, &mips3::LW); break;
//    case 0x27: fallback(opcode, &mips3::LWU); break;
//    case 0x37: fallback(opcode, &mips3::LD); break;
//    case 0x20: fallback(opcode, &mips3::LB); break;
//    case 0x21: fallback(opcode, &mips3::LH); break;
//    case 0x24: fallback(opcode, &mips3::LBU); break;
//    case 0x25: fallback(opcode, &mips3::LHU); break;

//    case 0x2B: fallback(opcode, &mips3::SW); break;
//    case 0x28: fallback(opcode, &mips3::SB); break;
//    case 0x29: fallback(opcode, &mips3::SH); break;
    case 0x23: LW(opcode); break;
    case 0x27: LWU(opcode); break;
    case 0x37: LD(opcode); break;
    case 0x20: LB(opcode); break;
    case 0x21: LH(opcode); break;
    case 0x24: LBU(opcode); break;
    case 0x25: LHU(opcode); break;

    case 0x2B: SW(opcode); break;
    case 0x28: SB(opcode); break;
    case 0x29: SH(opcode); break;
    case 0x3F: SD(opcode); break;

    case 0x1A: fallback(opcode, &mips3::LDL); break;
    case 0x1B: fallback(opcode, &mips3::LDR); break;
    case 0x22: fallback(opcode, &mips3::LWL); break;
    case 0x26: fallback(opcode, &mips3::LWR); break;


    case 0x2C: fallback(opcode, &mips3::SDL); break;
    case 0x2D: fallback(opcode, &mips3::SDR); break;
    case 0x31: fallback(opcode, &mips3::LWC1); break;
    case 0x39: fallback(opcode, &mips3::SWC1); break;



    default:
        qDebug() << QString::number(m_drc_pc, 16) << "Op:" << QString::number(opcode >> 26, 16);
        translate_failed();
        result = true;
        exit(-3);
        break;
    }

    return result;
}

void mips3_x64::check_icounter()
{
    inLocalLabel();
    mov(rax, ADR(m_icounter));
    mov(rax, ptr[rax]);
    cmp(rax, 0);
    jg(".l");
    set_next_pc(m_drc_pc);
    epilog();
    L(".l");
    outLocalLabel();
}

bool mips3_x64::cop1_fallback(uint32_t opcode)
{
    inLocalLabel();
    mov(rax, ADR(m_core->m_state.cpr[0][12]));
    mov(rax, ptr[rax]);

    mov(rcx, 1 << 26);
    test(rax, rcx);
    jne(".fr0");
    fallback(opcode, &mips3::cop1_execute_32);
    jmp(".end");
    L(".fr0");
    fallback(opcode, &mips3::cop1_execute_16);
    L(".end");

    outLocalLabel();
    return false;
}

void mips3_x64::set_next_pc(addr_t addr)
{
    mov(rcx, ADR(CORE_PC));
    mov(eax, addr);
    mov(qword[rcx], rax);
}

void mips3_x64::fallback(uint32_t opcode, void (mips3::*f)(uint32_t))
{
    // SysV AMD64 ABI - GNU
    mov(rdi, (size_t) m_core);
    mov(esi, opcode);
    mov(rax, (size_t) (void*)f);
    call(rax);
}

void mips3_x64::update_icounter()
{
    mov(rax, (size_t) m_block_icounter);
    mov(rcx, ADR(m_icounter));
    sub(ptr[rcx], rax);
    update_cp0_count();
    m_block_icounter = 0;
}

void mips3_x64::jmp_to_block(uint64_t addr)
{
    update_cp0_count();

    // Simple block linking
    void *next_ptr = get_block(addr);
    if (next_ptr) {
#if LOG_DYNAREC
        qDebug() << "Block link:" << QString::number(CORE_PC, 16) << "to" << QString::number(addr, 16);
#endif
        epilog(false);
        mov(rax, (size_t) next_ptr);
        jmp(rax);
        return;
    } else {
        set_next_pc(addr);
    }
    epilog();
}

void mips3_x64::jmp_to_register(int reg)
{
    update_cp0_count();

    mov(rcx, R_ref(reg));
    mov(ecx, ptr[rcx]);
    mov(rax, ADR(CORE_PC));
    mov(ptr[rax], rcx);
    epilog();
}

void mips3_x64::translate_failed()
{
    m_translate_failed = true;
    update_icounter();
    epilog();
}

void mips3_x64::update_cp0_count()
{
    mov(rax, (size_t) m_block_icounter * 20);
    mov(rcx, ADR(m_core->m_state.cpr[0][9]));
    add(ptr[rcx], rax);
}

int drc_main()
{
    try {
        mips3 core;
        mips3_x64 jit(&core);
        core.reset();

        for (int i = 0; i < 10; i++) {
            jit.run(1000);
        }
    } catch (exception& e) {
        cout << e.what();
    }

    return 0;
}
}


