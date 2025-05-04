// pipeline.c
#include "processor.h"
#include <stdio.h>

// ================== PIPELINE STAGES ================== //

// Stage 1: Instruction Fetch
void fetch(Processor *p) {
    if (p->PC < 1024 && !p->stall) {
        p->IF_ID.instr = p->instr_mem[p->PC];
        p->IF_ID.pc = p->PC;
        p->PC++;
        printf("[IF] Fetched 0x%04X from PC=0x%04X\n", p->IF_ID.instr, p->IF_ID.pc);
    }
}

// Stage 2: Instruction Decode
void decode(Processor *p) {
    if (p->IF_ID.instr && !p->stall) {
        p->ID_EX.instr = p->IF_ID.instr;
        p->ID_EX.pc = p->IF_ID.pc;
        
        // Decode common fields
        p->ID_EX.opcode = (p->IF_ID.instr >> 12) & 0xF;
        p->ID_EX.r1 = (p->IF_ID.instr >> 6) & 0x3F;
        
        // Decode based on format
        if (is_R_format(p->ID_EX.opcode)) {
            p->ID_EX.r2 = p->IF_ID.instr & 0x3F;
        } else { // I-format
            p->ID_EX.imm = p->IF_ID.instr & 0x3F;
            // Sign-extend 6-bit immediate
            if (p->ID_EX.imm & 0x20) p->ID_EX.imm |= 0xC0;
        }
        
        printf("[ID] Decoded opcode=0x%X, r1=%d, r2=%d, imm=%d\n", 
               p->ID_EX.opcode, p->ID_EX.r1, p->ID_EX.r2, p->ID_EX.imm);
    }
}

// Stage 3: Execute
void execute(Processor *p) {
    if (p->ID_EX.instr) {
        uint8_t result = 0;
        uint8_t op1 = p->R[p->ID_EX.r1];
        uint8_t op2 = is_R_format(p->ID_EX.opcode) ? p->R[p->ID_EX.r2] : p->ID_EX.imm;
        
        switch (p->ID_EX.opcode) {
            // ---- Arithmetic ----
            case 0x0: // ADD
                result = op1 + op2;
                p->R[p->ID_EX.r1] = result;
                update_flags(p, result, op1, op2, ADD);
                break;
                
            case 0x1: // SUB
                result = op1 - op2;
                p->R[p->ID_EX.r1] = result;
                update_flags(p, result, op1, op2, SUB);
                break;
                
            // ---- Data Movement ----
            case 0x3: // MOVI
                p->R[p->ID_EX.r1] = p->ID_EX.imm;
                break;
                
            case 0xA: // LDR (Load from memory)
                p->R[p->ID_EX.r1] = p->data_mem[p->ID_EX.imm];
                break;
                
            case 0xB: // STR (Store to memory)
                p->data_mem[p->ID_EX.imm] = p->R[p->ID_EX.r1];
                break;
                
            // ---- Control Flow ----
            case 0x4: // BEQZ
                if (p->R[p->ID_EX.r1] == 0) {
                    p->PC = p->ID_EX.pc + 1 + p->ID_EX.imm;
                    p->stall = 2; // Flush next 2 instructions
                }
                break;
                
            // ... Implement other opcodes (MUL, ANDI, etc.)
        }
        
        printf("[EX] Executed: R%d = 0x%02X\n", p->ID_EX.r1, result);
    }
}

// ================== PIPELINE CONTROL ================== //

void proc_cycle(Processor *p) {
    // Handle stalls (for branches)
    if (p->stall > 0) {
        p->stall--;
        if (p->stall == 0) {
            // Flush pipeline
            p->IF_ID.instr = 0;
            p->ID_EX.instr = 0;
        }
        return;
    }
    
    // Pipeline advances in reverse order
    execute(p);  // EX (stage 3)
    decode(p);   // ID (stage 2)
    fetch(p);    // IF (stage 1)
}

// ================== HELPER FUNCT