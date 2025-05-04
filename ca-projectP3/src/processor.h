#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define FLAG_C 0x08  
#define FLAG_V 0x04  
#define FLAG_N 0x02  
#define FLAG_S 0x01  
#define FLAG_Z 0x10  

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

typedef struct {
    uint16_t instr;
    uint16_t pc;
    uint8_t opcode;
    uint8_t r1, r2;
    int8_t imm;
} PipelineReg;

typedef struct {
    uint8_t R[64];          
    uint8_t SREG;           
    uint16_t PC;            

    uint16_t instr_mem[1024]; 
    uint8_t data_mem[2048];   

    PipelineReg IF_ID, ID_EX;
    int stall;              
} Processor;

void proc_init(Processor *p);
void proc_load_program(Processor *p, const char *filename);
void proc_cycle(Processor *p);
void print_registers(const Processor *p);
void print_pipeline(const Processor *p);

#endif 