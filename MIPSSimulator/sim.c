#include <stdio.h>
#include "shell.h"

void process_instruction()
{
    /* execute one instruction here. You should use CURRENT_STATE and modify
     * values in NEXT_STATE. You can call mem_read_32() and mem_write_32() to
     * access memory. */
    uint32_t op = mem_read_32(CURRENT_STATE.PC) >> 26;

    int instType = getInstType(op);

    switch (instType) {
        case 0:
            processRType();
            break;
        case 1:
            processIType();
            break;
        case 2:
            processJType(op);
            break;
    }
}

int getInstType(uint32_t op) {
    if (op == 0x0)
        return 0;
    else if (op == 0x3 || op == 0x2)
        return 2;
    else
        return 1;
}

void processIType() {

}

void processRType() {
    uint32_t inst = mem_read_32(CURRENT_STATE.PC);
    uint32_t funct = inst & 0x3F;
    uint32_t rs = inst >> 21, rt = (inst >> 16) & 0x1F, rd = (inst >> 11) & 0x1F;
    uint32_t sa = (inst >> 6) & 0x1F;
    uint32_t nRD = NEXT_STATE.REGS[rd], nRS = NEXT_STATE.REGS[rd], nRT = NEXT_STATE.REGS[rd];
    uint32_t cRT = CURRENT_STATE.REGS[rt], cRS = CURRENT_STATE.REGS[rs], cRD = CURRENT_STATE.REGS[rd];

    switch(funct) {
        case 0x9: // JARL
            if (rd == 0)
                rd = 31;
            NEXT_STATE.REGS[rd] = CURRENT_STATE.PC + 4;
            NEXT_STATE.PC = cRS;
            break;
        case 0x8: // JR
            NEXT_STATE.PC = cRS;
            break;

        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
        
        case 0xC: // SYSCALL
            if (CURRENT_STATE.REGS[2] == 0x0A)
                RUN_BIT = 0;
            break;
        case 0x00: // SLL
            nRD = (unsigned) cRT << sa;
            break;
        case 0x02: // SRL
            nRD = (unsigned) cRT >> sa;
            break;
        case 0x04: // SLLV
            nRD = (unsigned) cRT << cRS;
            break;
        case 0x06: // SRLV
            nRD = (unsigned) cRT >> cRS;
            break;
        case 0x03: // SRA
            if (cRT < 0 && sa > 0)
                nRD = cRT >> sa | ~(~0U >> sa);
            else
                nRD = cRT >> sa;
            break;
        case 0x07: // SRAV
            if (cRT < 0 && cRS > 0)
                nRD = cRT >> cRS | ~(~0U >> cRS);
            else
                nRD = cRT >> cRS;
            break;
        case 0x22: // SUB
            nRD = cRS - cRT;
            if (nRD + cRT != cRS)
                nRD = cRD;
            break;
        case 0x23: // SUBU
            nRD = cRS - cRT;
            break;
        case 0x2A: // SLT
            if (cRS < cRT)
                nRD = 1;
            else  
                nRD = 0;
            break;
        case 0x2B: // SLTU
            uint64_t cRS64 = cRS, cRT64 = cRT;
            if (cRS64 < cRT64)
                nRD = 1;
            else  
                nRD = 0;
            break;
        case 0x10: // MFHI
            nRD = CURRENT_STATE.HI;
            break;
        case 0x11: // MTHI
            NEXT_STATE.HI = cRS;
            break;
        case 0x12: // MFLO
            nRD = CURRENT_STATE.LO;
            break;
        case 0x13: // MTHI
            NEXT_STATE.LO = cRS;
            break;
        case 0x18: // MULT
            uint64_t mult = cRS * cRT;
            NEXT_STATE.LO = mult & 0xFFFFFFFF;
            NEXT_STATE.HI = mult >> 32;
            break;
        case 0x19: // MULTU
            uint64_t cRS64 = cRS, cRT64 = cRT;
            uint64_t mult = cRS64 * cRT64;
            NEXT_STATE.LO = mult & 0xFFFFFFFF;
            NEXT_STATE.HI = mult >> 32;
            break;
        case 0x1A: // DIV
            NEXT_STATE.LO = cRS / cRT;
            NEXT_STATE.HI = cRS % cRT;
            break;
        case 0x1B: // DIVU
            uint64_t cRS64 = cRS, cRT64 = cRT;
            NEXT_STATE.LO = cRS64 / cRT64;
            NEXT_STATE.HI = cRS64 % cRT64; 
            break;
        case 0x24: // AND
            nRD = cRS & cRT;
            break;
        case 0x20: // ADD
            nRD = cRS + cRT;
            if (nRD - cRT != cRS)
                nRD = cRD;
            break;
        case 0x21: // ADDU
            nRD = cRS + cRT;
            break;
        case 0x27: // NOR
            nRD = ~(cRS | cRT);
            break;
        case 0x25: // OR
            nRD = cRS | cRT;
            break;
        case 0x26: // XOR
            nRD = cRS ^ cRT;
            break;
    }
}

void processJType(op) {
    uint32_t pc4 = (CURRENT_STATE.PC & 0x1AF);
    uint32_t target = (mem_read_32(CURRENT_STATE.PC) << 6) >> 4;
    if (op == 3) {
        NEXT_STATE.REGS[31] = CURRENT_STATE.PC + 4;
    }
    NEXT_STATE.PC = pc4 + target;
}
