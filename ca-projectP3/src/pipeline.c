// pipeline.c
#include "processor.h"
#include <stdio.h>

// Update status flags after ALU operations
static void update_flags(Processor *p, uint8_t result, uint8_t op1, uint8_t op2, Opcode op) {
    // Zero flag
    p->SREG = (result == 0) ? (p->SREG | FLAG_Z) : (p->SREG & ~FLAG_Z);

    // Negative flag (MSB)
    p->SREG = (result & 0x80) ? (p->SREG | FLAG_N) : (p->SREG & ~FLAG_N);

    // Carry/Overflow for arithmetic ops
    if (op == OP_ADD || op == OP_SUB) {
        uint16_t tmp = (op == OP_ADD) ? (op1 + op2) : (op1 - op2);
        // Carry (bit 8)
        p->SREG = (tmp > 0xFF) ? (p->SREG | FLAG_C) : (p->SREG & ~FLAG_C);
        // Overflow (XOR last two carries)
        uint8_t overflow = ((op1 ^ result) & (op2 ^ result)) >> 7;
        p->SREG = overflow ? (p->SREG | FLAG_V) : (p->SREG & ~FLAG_V);
        // Sign (N XOR V)
        p->SREG = ((p->SREG & FLAG_N) ^ ((p->SREG & FLAG_V) << 1)) ? 
                 (p->SREG | FLAG_S) : (p->SREG & ~FLAG_S);
    }
}

// Instruction Fetch
static void fetch(Processor *p) {
    if (p->PC < 1024 && !p->stall) {
        p->IF_ID.instr = p->instr_mem[p->PC];
        p->IF_ID.pc = p->PC;
        p->PC++;
    }
}

// Instruction Decode
static void decode(Processor *p) {
    if (p->IF_ID.instr && !p->stall) {
        p->ID_EX = p->IF_ID; // Pass through pipeline
        
        // Decode opcode and fields
        p->ID_EX.opcode = (p->IF_ID.instr >> 12) & 0xF;
        p->ID_EX.r1 = (p->IF_ID.instr >> 6) & 0x3F;
        
        if (p->ID_EX.opcode <= OP_MUL || p->ID_EX.opcode == OP_EOR || p->ID_EX.opcode == OP_BR) {
            // R-format
            p->ID_EX.r2 = p->IF_ID.instr & 0x3F;
        } else {
            // I-format
            p->ID_EX.imm = p->IF_ID.instr & 0x3F;
            if (p->ID_EX.imm & 0x20) p->ID_EX.imm |= 0xC0; // Sign-extend
        }
    }
}

// Execute
static void execute(Processor *p) {
    if (!p->ID_EX.instr) return;

    uint8_t result = 0;
    uint8_t op1 = p->R[p->ID_EX.r1];
    uint8_t op2 = (p->ID_EX.opcode <= OP_MUL || p->ID_EX.opcode == OP_EOR || p->ID_EX.opcode == OP_BR) 
                 ? p->R[p->ID_EX.r2] : p->ID_EX.imm;

    switch (p->ID_EX.opcode) {
        case OP_ADD: // ADD R1 R2
            result = op1 + op2;
            p->R[p->ID_EX.r1] = result;
            update_flags(p, result, op1, op2, OP_ADD);
            break;

        case OP_MOVI: // MOVI R1 IMM
            p->R[p->ID_EX.r1] = p->ID_EX.imm;
            break;

        case OP_BEQZ: // BEQZ R1 IMM
            if (p->R[p->ID_EX.r1] == 0) {
                p->PC = p->ID_EX.pc + 1 + p->ID_EX.imm;
                p->stall = 2; // Flush pipeline
            }
            break;

        // Implement other instructions similarly...
    }
}

// Advance pipeline
void proc_cycle(Processor *p) {
    if (p->stall > 0) {
        p->stall--;
        if (p->stall == 0) {
            memset(&p->IF_ID, 0, sizeof(PipelineReg));
            memset(&p->ID_EX, 0, sizeof(PipelineReg));
        }
        return;
    }

    execute(p);
    decode(p);
    fetch(p);
}

// Print pipeline state
void print_pipeline(const Processor *p) {
    printf("PC: 0x%04X | IF: 0x%04X | ID: 0x%04X | EX: 0x%04X\n",
           p->PC, p->IF_ID.instr, p->ID_EX.instr, p->ID_EX.instr);
}

// Print registers
void print_registers(const Processor *p) {
    printf("Registers:\n");
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            int reg = i * 8 + j;
            printf("R%-2d: 0x%02X ", reg, p->R[reg]);
        }
        printf("\n");
    }
    printf("SREG: [%c%c%c%c%c]\n",
           (p->SREG & FLAG_C) ? 'C' : '-',
           (p->SREG & FLAG_V) ? 'V' : '-',
           (p->SREG & FLAG_N) ? 'N' : '-',
           (p->SREG & FLAG_S) ? 'S' : '-',
           (p->SREG & FLAG_Z) ? 'Z' : '-');
}