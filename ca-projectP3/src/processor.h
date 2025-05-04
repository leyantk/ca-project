// processor.h
#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

// Status register flags (SREG)
#define FLAG_C 0x08  // Carry
#define FLAG_V 0x04  // Overflow
#define FLAG_N 0x02  // Negative
#define FLAG_S 0x01  // Sign (N XOR V)
#define FLAG_Z 0x10  // Zero (custom position)

// Instruction opcodes
typedef enum {
    OP_ADD = 0x0,
    OP_SUB = 0x1,
    OP_MUL = 0x2,
    OP_MOVI = 0x3,
    OP_BEQZ = 0x4,
    OP_ANDI = 0x5,
    OP_EOR = 0x6,
    OP_BR = 0x7,
    OP_SAL = 0x8,
    OP_SAR = 0x9,
    OP_LDR = 0xA,
    OP_STR = 0xB
} Opcode;

// Pipeline registers
typedef struct {
    uint16_t instr;
    uint16_t pc;
    uint8_t opcode;
    uint8_t r1, r2;
    int8_t imm;
} PipelineReg;

// Processor state
typedef struct {
    // Registers
    uint8_t R[64];          // R0-R63
    uint8_t SREG;           // Status flags
    uint16_t PC;            // Program Counter

    // Harvard Memory
    uint16_t instr_mem[1024]; // Instruction memory (16-bit words)
    uint8_t data_mem[2048];   // Data memory (8-bit bytes)

    // Pipeline
    PipelineReg IF_ID, ID_EX;
    int stall;              // Stall counter for branches
} Processor;

// Function prototypes
void proc_init(Processor *p);
void proc_load_program(Processor *p, const char *filename);
void proc_cycle(Processor *p);
void print_registers(const Processor *p);
void print_pipeline(const Processor *p);

#endif // PROCESSOR_H