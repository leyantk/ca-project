#include "processor.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Initialize instruction and data memories to zero
void mem_init(Processor *p) {
    memset(p->instr_mem, 0, sizeof(p->instr_mem));
    memset(p->data_mem, 0, sizeof(p->data_mem));
}

// Load assembly program into instruction memory
void mem_load_program(Processor *p, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) { perror("fopen"); exit(EXIT_FAILURE); }

    char line[256];
    uint16_t addr = 0;
    while (fgets(line, sizeof(line), file) && addr < 1024) {
        if (line[0] == '\n' || line[0] == ';') continue;

        uint16_t instr = 0;
        unsigned r1, r2, imm;
        char label[64];

        // Parse each instruction line and encode into binary
        if (sscanf(line, "MOVI R%u %u", &r1, &imm) == 2) {
            instr = (0b0011 << 12) | ((r1 & 0b111111) << 6) | (imm & 0b111111);
        } else if (sscanf(line, "ADD R%u R%u", &r1, &r2) == 2) {
            instr = (0b0000 << 12) | ((r1 & 0b111111) << 6) | (r2 & 0b111111);
        } else if (sscanf(line, "SUB R%u R%u", &r1, &r2) == 2) {
            instr = (0b0001 << 12) | ((r1 & 0b111111) << 6) | (r2 & 0b111111);
        } else if (sscanf(line, "MUL R%u R%u", &r1, &r2) == 2) {
            instr = (0b0010 << 12) | ((r1 & 0b111111) << 6) | (r2 & 0b111111);
        } else if (sscanf(line, "ANDI R%u %u", &r1, &imm) == 2) {
            instr = (0b0101 << 12) | ((r1 & 0b111111) << 6) | (imm & 0b111111);
        } else if (sscanf(line, "EOR R%u R%u", &r1, &r2) == 2) {
            instr = (0b0110 << 12) | ((r1 & 0b111111) << 6) | (r2 & 0b111111);
        } else if (sscanf(line, "SAL R%u %u", &r1, &imm) == 2) {
            instr = (0b1000 << 12) | ((r1 & 0b111111) << 6) | (imm & 0b111111);
        } else if (sscanf(line, "SAR R%u %u", &r1, &imm) == 2) {
            instr = (0b1001 << 12) | ((r1 & 0b111111) << 6) | (imm & 0b111111);
        } else if (sscanf(line, "BEQZ R%u %u", &r1, &imm) == 2) {
            instr = (0b0100 << 12) | ((r1 & 0b111111) << 6) | (imm & 0b111111);
        } else if (sscanf(line, "BR R%u R%u", &r1, &r2) == 2) {
            instr = (0b0111 << 12) | ((r1 & 0b111111) << 6) | (r2 & 0b111111);
        } else if (sscanf(line, "LDR R%u , %63s", &r1, label) == 2) {
            instr = (0b1010 << 12) | ((r1 & 0b111111) << 6) | (imm & 0b111111);
        } else if (sscanf(line, "STR R%u , %63s", &r1, label) == 2) {
            instr = (0b1011 << 12) | ((r1 & 0b111111) << 6) | (imm & 0b111111);
        } else {
            fprintf(stderr, "Unknown instruction: %s", line);
            exit(EXIT_FAILURE);
        }

        p->instr_mem[addr++] = instr;
    }
    fclose(file);
}

// Read a byte from data memory
uint8_t mem_read_data(Processor *p, uint16_t addr) {
    if (addr >= 2048) return 0;
    return p->data_mem[addr];
}

// Write a byte to data memory
void mem_write_data(Processor *p, uint16_t addr, uint8_t data) {
    if (addr >= 2048) return;
    p->data_mem[addr] = data;
}

// Print non-zero contents of instruction memory
void mem_print_instr(const Processor *p) {
    printf("Instruction Memory:\n");
    for (int i = 0; i < 1024; i++) {
        if (p->instr_mem[i] != 0) {
            printf("0x%04X: 0x%04X\n", i, p->instr_mem[i]);
        }
    }
}

// Print non-zero contents of data memory
void mem_print_data(const Processor *p) {
    printf("Data Memory:\n");
    for (int i = 0; i < 2048; i++) {
        if (p->data_mem[i] != 0) {
            printf("0x%04X: 0x%02X\n", i, p->data_mem[i]);
        }
    }
}