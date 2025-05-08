#include "processor.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// === MEMORY LOAD ===
void mem_init(Processor *p) {
    memset(p->instr_mem, 0, sizeof(p->instr_mem));
    memset(p->data_mem, 0, sizeof(p->data_mem));
}

void mem_load_program(Processor *p, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) { perror("fopen"); exit(EXIT_FAILURE); }

    char line[128];
    uint16_t addr = 0;
    while (fgets(line, sizeof(line), file) && addr < 1024) {
        if (line[0] == '\n' || line[0] == ';' || line[0] == '#') continue;
        uint16_t opcode = 0, rs = 0, rt = 0, imm = 0;
        char op[16]; int r1, r2, value;

        if (sscanf(line, "%s R%d R%d", op, &r1, &r2) == 3) {
            rs = (uint8_t)r1; rt = (uint8_t)r2;
            if      (!strcmp(op, "ADD")) opcode = 0;
            else if (!strcmp(op, "SUB")) opcode = 1;
            else if (!strcmp(op, "MUL")) opcode = 2;
            else if (!strcmp(op, "EOR")) opcode = 6;
            else if (!strcmp(op, "BR"))  opcode = 7;
        } else if (sscanf(line, "%s R%d %d", op, &r1, &value) == 3) {
            rs = (uint8_t)r1; imm = (uint8_t)value;
            if      (!strcmp(op, "MOVI")) opcode = 3;
            else if (!strcmp(op, "ANDI")) opcode = 5;
            else if (!strcmp(op, "SAL"))  opcode = 8;
            else if (!strcmp(op, "SAR"))  opcode = 9;
            else if (!strcmp(op, "BEQZ")) opcode = 4;
            else if (!strcmp(op, "LDR"))  opcode = 10;
            else if (!strcmp(op, "STR"))  opcode = 11;
        } else {
            fprintf(stderr, "Invalid instruction format: %s", line);
            exit(EXIT_FAILURE);
        }

        uint16_t instr = (opcode << 12) | ((rs & 0x3F) << 6) | (opcode >= 3 ? (imm & 0x3F) : (rt & 0x3F));
        p->instr_mem[addr++] = instr;
    }
    fclose(file);
}

uint8_t mem_read_data(Processor *p, uint16_t addr) {
    if (addr >= 2048) return 0;
    return p->data_mem[addr];
}

void mem_write_data(Processor *p, uint16_t addr, uint8_t data) {
    if (addr >= 2048) return;
    p->data_mem[addr] = data;
}

void mem_print_instr(const Processor *p) {
    printf("Instruction Memory:\n");
    for (int i = 0; i < 1024; i++) {
        if (p->instr_mem[i])
            printf("0x%04X: 0x%04X\n", i, p->instr_mem[i]);
    }
}

void mem_print_data(const Processor *p) {
    printf("Data Memory:\n");
    for (int i = 0; i < 2048; i++) {
        if (p->data_mem[i])
            printf("0x%04X: 0x%02X\n", i, p->data_mem[i]);
    }
}