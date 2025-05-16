#include "processor.h"
#include <string.h>
#include <stdio.h>

void proc_init(Processor *p) {
    memset(p, 0, sizeof(Processor));
    p->PC = 0; 
    p->IF_ID.valid = false;
    p->ID_EX.valid = false;
}





