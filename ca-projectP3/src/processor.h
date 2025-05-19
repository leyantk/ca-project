#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <stdint.h>
#include <stdbool.h>
#include <stdint.h>

#define FLAG_C 0x08  // carry flag
#define FLAG_V 0x04  // overflow
#define FLAG_N 0x02  // negative
#define FLAG_S 0x01  // sign
#define FLAG_Z 0x10  // zero

typedef struct {
    short int instr;
    short int pc;
    bool     valid;
} IF_ID_Reg;

typedef struct {
    short int instr;
    short int pc;
    int8_t  opcode;
    int8_t  rs;
    int8_t  rt;
    short int  imm;
    int8_t  valueRS;
    int8_t  valueRT;
    bool     valid;
} ID_EX_Reg;

typedef struct {
    int8_t      Register[64];
    int8_t      SREG;
    short int     PC;
    bool    branchTaken;


    short int     instr_mem[1024];
    int8_t      data_mem[2048];

    IF_ID_Reg    IF_ID;
    ID_EX_Reg    ID_EX;
    short int     EX_instr;
    short int     EX_pc;
    bool         EX_valid;
} Processor;

void proc_init(Processor *p);
void mem_init(Processor *p);
void mem_load_program(Processor *p, const char *filename);
int8_t mem_read_data(Processor *p, short int addr);
void mem_write_data(Processor *p, short int addr, int8_t data);
void mem_print_instr(const Processor *p);
void mem_print_data(const Processor *p);
void process_cycle(Processor *p);
void print_registers(const Processor *p);
void print_pipeline(const Processor *p, int cycle);
#endif
