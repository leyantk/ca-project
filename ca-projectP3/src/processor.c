// processor.c
#include "processor.h"
#include <stdio.h>
#include <stdlib.h>

// Initialize processor state
void proc_init(Processor *p) {
    memset(p->R, 0, 64);                    // Clear registers
    p->SREG = 0;                            // Clear flags
    p->PC = 0;                              // Start at address 0
    memset(p->instr_mem, 0, 1024 * sizeof(uint16_t)); // Clear instruction memory
    memset(p->data_mem, 0, 2048);           // Clear data memory
    memset(&p->IF_ID, 0, sizeof(PipelineReg));
    memset(&p->ID_EX, 0, sizeof(PipelineReg));
    p->stall = 0;
}

// Load program into instruction memory
void proc_load_program(Processor *p, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open program file");
        exit(EXIT_FAILURE);
    }

    uint16_t addr = 0;
    char line[256];
    while (fgets(line, sizeof(line), file) && addr < 1024) {
        // Skip comments and empty lines
        if (line[0] == ';' || line[0] == '\n') continue;

        // Simple assembler (supports MOVI, ADD, etc.)
        uint16_t instr = 0;
        if (sscanf(line, "MOVI R%d %d", &instr, &instr) == 2) {
            p->instr_mem[addr++] = (0x3 << 12) | ((instr & 0x3F) << 6) | (instr & 0x3F);
        } 
        // Add more instructions here as needed
    }

    fclose(file);
}