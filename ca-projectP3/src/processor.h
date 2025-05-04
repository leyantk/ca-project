// processor.h
#include <stdint.h>

typedef struct {
    // Registers
    uint8_t R[64];          // R0-R63 (8-bit)
    uint8_t SREG;           // Flags: C (bit 4), V (bit 3), N (bit 2), S (bit 1), Z (bit 0)
    uint16_t PC;            // 16-bit program counter

    // Harvard Memory
    uint16_t instr_mem[1024]; // 16-bit instruction memory
    uint8_t data_mem[2048];   // 8-bit data memory

    // Pipeline registers (IF/ID, ID/EX)
    typedef struct {
        uint16_t instr;  // Raw instruction (16-bit)
        uint16_t pc;     // Address of this instruction
    } IF_ID;

    // Extended version (add fields as you decode):
typedef struct {
    uint16_t instr;  // Original instruction
    uint16_t pc;     // PC value (for branches)
    uint8_t opcode;  // Decoded opcode (4-bit)
    uint8_t r1, r2;  // Register indices (6-bit)
    uint8_t imm;     // Immediate value (6-bit)
} ID_EX;
} Processor;

// Function prototypes
void proc_init(Processor *p);
void proc_load_program(Processor *p, const char *filename);
void proc_cycle(Processor *p);