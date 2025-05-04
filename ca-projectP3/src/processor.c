#include "processor.h"
#include <stdio.h>
#include <stdlib.h>
//initialize processor
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
            instr = (0b0011 << 12) | ((r1 & 0b111111)<<6) | (imm & 0b111111);
        }
        else if (sscanf(line, "ADD R%u R%u", &r1, &r2) == 2) {
            instr = (0b0000 << 12) | ((r1&0b111111)<<6) | (r2&0b111111);
        }
        else if (sscanf(line, "SUB R%u R%u", &r1, &r2) == 2) {
            instr = (0b0001 << 12) | ((r1&0b111111)<<6) | (r2&0b111111);
        }
        else if (sscanf(line, "MUL R%u R%u", &r1, &r2) == 2) {
            instr = (0b0010 << 12) | ((r1&0b111111)<<6) | (r2&0b111111);
        }
        else if (sscanf(line, "ANDI R%u %u", &r1, &imm) == 2) {
            instr = (0b0101 << 12) | ((r1&0b111111)<<6) | (imm & 0b111111);
        }
        else if (sscanf(line, "EOR R%u R%u", &r1, &r2) == 2) {
            instr = (0b0110 << 12) | ((r1&0b111111)<<6) | (r2&0b111111);
        }
        else if (sscanf(line, "SAL R%u %u", &r1, &imm) == 2) {
            instr = (0b1000 << 12) | ((r1&0b111111)<<6) | (imm & 0b111111);
        }
        else if (sscanf(line, "SAR R%u %u", &r1, &imm) == 2) {
            instr = (0b1001 << 12) | ((r1&0b111111)<<6) | (imm & 0b111111);
        }
        else if (sscanf(line, "BEQZ R%u %u", &r1, &imm) == 2) {
            instr = (0b0100 << 12) | ((r1&0b111111)<<6) | (imm & 0b111111);
        }
        else if (sscanf(line, "BR R%u R%u", &r1, &r2) == 2) {
            instr = (0b0111<< 12) | ((r1&0b111111)<<6) | (r2&0b111111);
        }
        else if (sscanf(line, "LDR R%u , %63s", &r1, label) == 2) {
            instr = (0b1010 << 12) | ((r1&0b111111)<<6) | (imm & 0b111111);
        }
        else if (sscanf(line, "STR R%u , %63s", &r1, label) == 2) {
            instr = (0b1011 << 12) | ((r1&0b111111)<<6) | (imm & 0b111111);
        }
        else {
            fprintf(stderr, "Unknown instr: %s", line);
            exit(EXIT_FAILURE);
        }

        p->instr_mem[addr++] = instr;
    }
    fclose(file);
}
