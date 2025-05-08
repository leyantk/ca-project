#include "processor.h"
#include <string.h>

void proc_init(Processor *p) {
    memset(p, 0, sizeof(Processor));
    p->PC = 0x020F;  // Initialize PC to 0x020F
    p->IF_ID.valid = false;
    p->ID_EX.valid = false;
}
