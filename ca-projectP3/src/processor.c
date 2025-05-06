#include "processor.h"
#include <stdio.h>
#include <stdlib.h>
//initialize processor
void proc_init(Processor *p) {
    memset(p->R, 0, 64);
    p->SREG = 0;
    p->PC = 0;
    mem_init(p); // Initialize memories
    memset(&p->IF_ID, 0, sizeof(PipelineReg));
    memset(&p->ID_EX, 0, sizeof(PipelineReg));
    p->stall = 0;
}


