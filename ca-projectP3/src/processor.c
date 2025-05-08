#include "processor.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h> // Needed for memset

// Initialize processor
void proc_init(Processor *p) {
    memset(p->R, 0, sizeof(p->R));
    p->SREG = 0;
    p->PC = 0;

    // Initialize memories
    memset(p->instr_mem, 0, sizeof(p->instr_mem));
    memset(p->data_mem, 0, sizeof(p->data_mem));

    // Clear pipeline registers
    memset(&p->IF_ID, 0, sizeof(IF_ID_Reg));
    memset(&p->ID_EX, 0, sizeof(ID_EX_Reg));

    p->stall = 0;
}
