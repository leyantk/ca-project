#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <stdint.h>
#include <stdbool.h>

#define FLAG_C 0x08  /* Carry flag  */
#define FLAG_V 0x04  /* Overflow flag */
#define FLAG_N 0x02  /* Negative flag */
#define FLAG_S 0x01  /* Sign flag */
#define FLAG_Z 0x10  /* Zero flag     */

// IF/ID pipeline latch
typedef struct {
    uint16_t instr;   // raw instruction bits
    uint16_t pc;      // PC of fetched instruction
    bool     valid;   // valid bit
} IF_ID_Reg;

// ID/EX pipeline latch
typedef struct {
    uint16_t instr;   // raw instruction bits
    uint16_t pc;      // PC of this instruction

    uint8_t  opcode;  // bits [15:12]
    uint8_t  rs;      // bits [11:6]
    uint8_t  rt;      // bits [5:0]
    int16_t  imm;     // sign-extended bits [5:0]
    uint16_t address; // bits [11:0]
    uint8_t  rd;      // bits [5:2]
    uint8_t  shamt;   // bits [5:0]
    uint8_t  funct;   // bits [3:0]

    int16_t  valueRS; // register file value at rs
    int16_t  valueRT; // register file value at rt
} ID_EX_Reg;

// Processor state and pipeline registers
typedef struct {
    uint8_t      R[64];           // general-purpose registers
    uint8_t      SREG;            // status register
    uint16_t     PC;              // program counter

    uint16_t     instr_mem[1024]; // instruction memory (1K × 16-bit)
    uint8_t      data_mem[2048];  // data memory (2K × 8-bit)

    IF_ID_Reg    IF_ID;           // IF→ID latch
    ID_EX_Reg    ID_EX;           // ID→EX latch

    int          stall;           // stall counter
} Processor;

// Initialize the processor state
void proc_init(Processor *p);

// Load program into instruction memory
void proc_load_program(Processor *p, const char *filename);

// Perform one fetch-decode-execute cycle
void proc_cycle(Processor *p);

// Print register contents
void print_registers(const Processor *p);

// Print pipeline registers (for debugging)
void print_pipeline(const Processor *p);

// Memory access helpers
uint8_t mem_read_data(Processor *p, uint16_t addr);
void    mem_write_data(Processor *p, uint16_t addr, uint8_t data);

#endif /* PROCESSOR_H */
