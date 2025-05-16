#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <stdint.h>
#include <stdbool.h>

#define FLAG_C 0x08  // carry flag
#define FLAG_V 0x04  // overflow
#define FLAG_N 0x02  // negative
#define FLAG_S 0x01  // sign
#define FLAG_Z 0x10  // zero

typedef struct {
    uint16_t instr;
    uint16_t pc;
    bool     valid;
} IF_ID_Reg;

typedef struct {
    uint16_t instr;
    uint16_t pc;
    uint8_t  opcode;
    uint8_t  rs;
    uint8_t  rt;
    int16_t  imm;
    uint8_t  valueRS;
    uint8_t  valueRT;
    bool     valid;
} ID_EX_Reg;

typedef struct {
    uint8_t      R[64];
    uint8_t      SREG;
    uint16_t     PC;

    uint16_t     instr_mem[1024];
    uint8_t      data_mem[2048];

    IF_ID_Reg    IF_ID;
    ID_EX_Reg    ID_EX;
    uint16_t     EX_instr;
    uint16_t     EX_pc;
    bool         EX_valid;
} Processor;

void proc_init(Processor *p);
void mem_init(Processor *p);
void mem_load_program(Processor *p, const char *filename);
uint8_t mem_read_data(Processor *p, uint16_t addr);
void mem_write_data(Processor *p, uint16_t addr, uint8_t data);
void mem_print_instr(const Processor *p);
void mem_print_data(const Processor *p);
void processa_cycle(Processor *p);
void print_registers(const Processor *p);
void print_pipeline(const Processor *p, int cycle);
#endif
