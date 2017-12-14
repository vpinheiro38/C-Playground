#include <stdio.h>
#include <math.h>
#include "shell.h"

void processRType();
void processIType();
void processJType();
void processREGIMMType();
int getInstType();
uint32_t signExtend16();
uint64_t signExtend32();

uint32_t inst;
uint32_t op;
uint32_t funct;
uint32_t rs, rt, rd;
uint32_t sa;
uint32_t cRT, cRS, cRD, cPC;

uint32_t offset16;
uint32_t regimm;

int pcPlus4;

void process_instruction()
{
    /* execute one instruction here. You should use CURRENT_STATE and modify
     * values in NEXT_STATE. You can call mem_read_32() and mem_write_32() to
     * access memory. */

    inst = mem_read_32(CURRENT_STATE.PC);
    rs = (inst >> 21) & 0x0000001F, rt = (inst >> 16) & 0x0000001F, rd = (inst >> 11) & 0x0000001F;
    op = inst >> 26;
    funct = inst & 0x3F;
    sa = (inst >> 6) & 0x1F;
    cRT = CURRENT_STATE.REGS[rt], cRS = CURRENT_STATE.REGS[rs], cRD = CURRENT_STATE.REGS[rd];

    offset16 = inst & 0xFFFF;
    regimm = (inst >> 16) & 0x1F;

    cPC = CURRENT_STATE.PC;

    int instType = getInstType(op);
    pcPlus4 = 1;

    switch (instType) {
        case 0:
            processRType();
            break;
        case 1:
            processIType();
            break;
        case 2:
            processJType();
            break;
    }
}

int getInstType() {
    if (op == 0x0)
        return 0;
    else if (op == 0x3 || op == 0x2)
        return 2;
    else
        return 1;
}

void processIType() {
    uint32_t target = signExtend16(offset16);
    target << 2;
    
    switch(op) {
        case 0x1:
            processREGIMMType();
            break;
        case 0x4: // BEQ
            if (cRS == cRT) {
                NEXT_STATE.PC = cPC + target + 4;
                pcPlus4 = 0;
                printf("EQUAL - ");
            }
            break;
        case 0x5: // BNE
            if (cRS != cRT) {
                NEXT_STATE.PC = cPC + target + 4;
                pcPlus4 = 0;
            }
            break;
        case 0x6: // BLEZ
            if (cRS >> 31 == 1 || cRS == 0){
                NEXT_STATE.PC = cPC + target + 4;
                pcPlus4 = 0;
            }
            break;
        case 0x7: // BGTZ
            if (cRS >> 31 == 0 && cRS != 0){
                NEXT_STATE.PC = cPC + target + 4;
                pcPlus4 = 0;
            }
                
            break;

        case 0x8: // ADDI
            if (offset16 >> 15 == 1)
                NEXT_STATE.REGS[rt] = cRS + (offset16 | 0xFFFF0000);
            else
                NEXT_STATE.REGS[rt] = cRS + offset16;
            break;
        case 0x9: // ADDIU
            if (offset16 >> 15 == 1)
                NEXT_STATE.REGS[rt] = cRS + (offset16 | 0xFFFF0000);
            else
                NEXT_STATE.REGS[rt] = cRS + offset16;
            break;
        case 0xC: // ANDI
            NEXT_STATE.REGS[rt] = ((cRS << 15) >> 15) & offset16;
            break;
        case 0x20: // LB
            NEXT_STATE.REGS[rt] = mem_read_32(cRS + signExtend16(offset16)) & 0xFF;
            if (NEXT_STATE.REGS[rt] >> 7 == 1)
                NEXT_STATE.REGS[rt] = NEXT_STATE.REGS[rt] | 0xFFFFFF00;
            break;
        case 0x24: // LBU
            NEXT_STATE.REGS[rt] = mem_read_32(cRS + signExtend16(offset16)) & 0xFF;
            break;
        case 0x21: // LH
            NEXT_STATE.REGS[rt] = mem_read_32(cRS + signExtend16(offset16)) & 0xFFFF;
            if (NEXT_STATE.REGS[rt] >> 15 == 1)
                NEXT_STATE.REGS[rt] = NEXT_STATE.REGS[rt] | 0xFFFF0000;
            break;
        case 0x25: // LHU
            NEXT_STATE.REGS[rt] = mem_read_32(cRS + signExtend16(offset16)) & 0xFFFF;
            break;
        case 0x23: // LW
            NEXT_STATE.REGS[rt] = mem_read_32(cRS + signExtend16(offset16));
            break;
        case 0x28: // SB
            mem_write_32(cRS + signExtend16(offset16), cRT & 0xFF);
            break;
        case 0x29: // SH
            mem_write_32(cRS + signExtend16(offset16), cRT & 0xFFFF);
            break;
        case 0x2B: // SW
            mem_write_32(cRS + signExtend16(offset16), cRT);
            break;
        case 0xF: // LUI
            NEXT_STATE.REGS[rt] = offset16 << 16;
            break;
        case 0xD: // ORI
            NEXT_STATE.REGS[rt] = offset16 | cRS;
            break;
        case 0xA: // SLTI
            if (offset16 >> 15 == 1)
                if (cRS < (offset16 | 0xFFFF0000)) {
                    NEXT_STATE.REGS[rd] = 1;
                } else {
                    NEXT_STATE.REGS[rd] = 0;
                }
            else {
                if (cRS < offset16) {
                    NEXT_STATE.REGS[rd] = 1;
                } else {
                    NEXT_STATE.REGS[rd] = 0;
                }
            }
            break;
        case 0xB: // SLTIU
            if (offset16 >> 15 == 1)
                if (cRS < (offset16 | 0xFFFF0000)) {
                    NEXT_STATE.REGS[rd] = 1;
                } else {
                    NEXT_STATE.REGS[rd] = 0;
                }
            else {
                if (cRS < offset16) {
                    NEXT_STATE.REGS[rd] = 1;
                } else {
                    NEXT_STATE.REGS[rd] = 0;
                }
            }
            break;
        case 0xE: // XORI
            NEXT_STATE.REGS[rt] = cRS ^ offset16;
            break;
    }

    if (pcPlus4)
        NEXT_STATE.PC = cPC + 4;
}

void processRType() {
    uint64_t mult = signExtend32(cRS) * signExtend32(cRT);

    switch(funct) {
        case 0x9: // JARL
            if (rd == 0)
                rd = 31;
            NEXT_STATE.REGS[rd] = cPC + 4;
            NEXT_STATE.PC = cRS;
            pcPlus4 = 0;
            break;
        case 0x8: // JR
            NEXT_STATE.PC = cRS;
            pcPlus4 = 0;
            break;
        
        case 0xC: // SYSCALL
            if (CURRENT_STATE.REGS[2] == 0x0A)
                RUN_BIT = 0;
            break;
        case 0x00: // SLL
            NEXT_STATE.REGS[rd] = cRT << sa;
            break;
        case 0x02: // SRL
            NEXT_STATE.REGS[rd] = cRT >> sa;
            break;
        case 0x04: // SLLV
            NEXT_STATE.REGS[rd] = cRT << cRS;
            break;
        case 0x06: // SRLV
            NEXT_STATE.REGS[rd] = cRT >> cRS;
            break;
        case 0x03: // SRA
            if (cRT >> 31 == 1)
                NEXT_STATE.REGS[rd] = cRT >> sa | (((uint32_t) pow(2, sa) - 1) << (32-sa));
            else
                NEXT_STATE.REGS[rd] = cRT >> sa;
            break;
        case 0x07: // SRAV
            if (cRT < 0 && cRS > 0)
                NEXT_STATE.REGS[rd] = cRT >> cRS | (((uint32_t) pow(2, cRS) - 1) << (32-cRS));
            else
                NEXT_STATE.REGS[rd] = cRT >> cRS;
            break;
        case 0x22: // SUB
            NEXT_STATE.REGS[rd] = cRS - cRT;
            break;
        case 0x23: // SUBU
            NEXT_STATE.REGS[rd] = cRS - cRT;
            break;
        case 0x2A: // SLT
            if (cRS < cRT)
                NEXT_STATE.REGS[rd] = 1;
            else  
                NEXT_STATE.REGS[rd] = 0;
            break;
        case 0x2B: // SLTU
            if (cRS < cRT)
                NEXT_STATE.REGS[rd] = 1;
            else  
                NEXT_STATE.REGS[rd] = 0;
            break;
        case 0x10: // MFHI
            NEXT_STATE.REGS[rd] = CURRENT_STATE.HI;
            break;
        case 0x11: // MTHI
            NEXT_STATE.HI = cRS;
            break;
        case 0x12: // MFLO
            NEXT_STATE.REGS[rd] = CURRENT_STATE.LO;
            break;
        case 0x13: // MTHI
            NEXT_STATE.LO = cRS;
            break;
        case 0x18: // MULT
            NEXT_STATE.LO = mult & 0xFFFFFFFF;
            NEXT_STATE.HI = mult >> 32;
            break;
        case 0x19: // MULTU
            NEXT_STATE.LO = mult & 0xFFFFFFFF;
            NEXT_STATE.HI = mult >> 32;
            break;
        case 0x1A: // DIV
            NEXT_STATE.LO = cRS / cRT;
            NEXT_STATE.HI = cRS % cRT;
            break;
        case 0x1B: // DIVU
            NEXT_STATE.LO = cRS / cRT;
            NEXT_STATE.HI = cRS % cRT;
            break;
        case 0x24: // AND
            NEXT_STATE.REGS[rd] = cRS & cRT;
            break;
        case 0x20: // ADD
            NEXT_STATE.REGS[rd] = cRS + cRT;
            break;
        case 0x21: // ADDU
            NEXT_STATE.REGS[rd] = cRS + cRT;
            break;
        case 0x27: // NOR
            NEXT_STATE.REGS[rd] = ~(cRS | cRT);
            break;
        case 0x25: // OR
            NEXT_STATE.REGS[rd] = cRS | cRT;
            break;
        case 0x26: // XOR
            NEXT_STATE.REGS[rd] = cRS ^ cRT;
            break;
    }

    if (pcPlus4)
        NEXT_STATE.PC = cPC + 4;
}

void processJType() {
    uint32_t pc4 = (cPC & 0xF0000000);
    uint32_t target = (inst << 6) >> 4;
    if (op == 3) {
        NEXT_STATE.REGS[31] = cPC + 4;
    }
    NEXT_STATE.PC = pc4 + target;
    pcPlus4 = 0;
}

void processREGIMMType() {
    uint32_t target = signExtend16(offset16);
    target << 2;

    switch(regimm) {
        case 0x0: // BLTZ
            if (cRS >> 31 == 1){
                NEXT_STATE.PC = cPC + target + 4;  
                pcPlus4 = 0;
            }
            break;
        case 0x1: // BGEZ
            if (cRS >> 31 == 0){
                NEXT_STATE.PC = cPC + target + 4;
                pcPlus4 = 0;
            }
            break;
        case 0x11: // BGEZAL
            if (cRS >> 31 == 0){
                NEXT_STATE.PC = cPC + target;   
                NEXT_STATE.REGS[31] = cPC + 4 + 4;  
                pcPlus4 = 0;
            }       
            break;
        case 0x10: // BLTZAL
            if (cRS >> 31 == 1){
                NEXT_STATE.PC = cPC + target + 4;
                NEXT_STATE.REGS[31] = cPC + 4;   
                pcPlus4 = 0;
            }
            break;
    }
}

uint32_t signExtend16(uint32_t num) {
    if (num >> 15 == 1) {
        return num | 0xFFFF0000;
    } 
    return num;
}

uint64_t signExtend32(uint32_t num) {
    if (num >> 31 == 1) {
        return num | 0xFFFFFFFF00000000;
    } 
    return num;
}