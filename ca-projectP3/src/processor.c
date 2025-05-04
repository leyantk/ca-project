#include "processor.h"
#include <stdio.h>
#include <stdlib.h>

void proc_init(Processor *p) {
    memset(p->R, 0, 64);                    
    p->SREG = 0;                            
    p->PC = 0;                              
    memset(p->instr_mem, 0, 1024 * sizeof(uint16_t)); 
    memset(p->data_mem, 0, 2048);           
    memset(&p->IF_ID, 0, sizeof(PipelineReg));
    memset(&p->ID_EX, 0, sizeof(PipelineReg));
    p->stall = 0;
}

void proc_load_program(Processor *p, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) { perror("open"); exit(EXIT_FAILURE); }

    char line[256];
    uint16_t addr = 0;
    while (fgets(line, sizeof(line), file) && addr < 1024) {
        if (line[0]=='\n' || line[0]==';') continue;

        uint16_t instr = 0;
        unsigned r1, r2, imm;
        char label[64];

        if (sscanf(line, "MOVI R%u %u", &r1, &imm) == 2) {
            instr = (OP_MOVI << 12) | ((r1 & 0x3F)<<6) | (imm & 0x3F);
        }
        else if (sscanf(line, "ADD R%u R%u", &r1, &r2) == 2) {
            instr = (OP_ADD << 12) | ((r1&0x3F)<<6) | (r2&0x3F);
        }
        else if (sscanf(line, "SUB R%u R%u", &r1, &r2) == 2) {
            instr = (OP_SUB << 12) | ((r1&0x3F)<<6) | (r2&0x3F);
        }
        else if (sscanf(line, "MUL R%u R%u", &r1, &r2) == 2) {
            instr = (OP_MUL << 12) | ((r1&0x3F)<<6) | (r2&0x3F);
        }
        else if (sscanf(line, "ANDI R%u %u", &r1, &imm) == 2) {
            instr = (OP_ANDI << 12) | ((r1&0x3F)<<6) | (imm & 0x3F);
        }
        else if (sscanf(line, "EOR R%u R%u", &r1, &r2) == 2) {
            instr = (OP_EOR << 12) | ((r1&0x3F)<<6) | (r2&0x3F);
        }
        else if (sscanf(line, "SAL R%u %u", &r1, &imm) == 2) {
            instr = (OP_SAL << 12) | ((r1&0x3F)<<6) | (imm & 0x3F);
        }
        else if (sscanf(line, "SAR R%u %u", &r1, &imm) == 2) {
            instr = (OP_SAR << 12) | ((r1&0x3F)<<6) | (imm & 0x3F);
        }
        else if (sscanf(line, "BEQZ R%u %u", &r1, &imm) == 2) {
            instr = (OP_BEQZ << 12) | ((r1&0x3F)<<6) | (imm & 0x3F);
        }
        else if (sscanf(line, "BR R%u R%u", &r1, &r2) == 2) {
            instr = (OP_BR << 12) | ((r1&0x3F)<<6) | (r2&0x3F);
        }
        else if (sscanf(line, "LDR R%u , %63s", &r1, label) == 2) {
            instr = (OP_LDR << 12) | ((r1&0x3F)<<6) | (imm & 0x3F);
        }
        else if (sscanf(line, "STR R%u , %63s", &r1, label) == 2) {
            instr = (OP_STR << 12) | ((r1&0x3F)<<6) | (imm & 0x3F);
        }
        else {
            fprintf(stderr, "Unknown instr: %s", line);
            exit(EXIT_FAILURE);
        }

        p->instr_mem[addr++] = instr;
    }
    fclose(file);
}
