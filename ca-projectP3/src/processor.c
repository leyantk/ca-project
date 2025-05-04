// processor.c
#include "processor.h"
#include <stdint.h>

void proc_init(Processor *p) {
    memset(p->R, 0, 64);       // Zero all registers
    p->SREG = 0;               // Clear flags
    p->PC = 0;                 // Start execution at address 0
    memset(p->instr_mem, 0, 1024 * sizeof(uint16_t));
    memset(p->data_mem, 0, 2048);
    p->IF_ID.instr = 0;
    p->ID_EX.instr = 0;
}