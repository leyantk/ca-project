#include "processor.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


void mem_init(Processor *p) {
    memset(p->instr_mem, 0, sizeof(p->instr_mem));
    memset(p->data_mem, 0, sizeof(p->data_mem));
}

void mem_load_program(Processor *p, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    printf("Opening file: %s\n", filename);

    char line[128];
    uint16_t addr = 0; 

    while (fgets(line, sizeof(line), file) && addr < 0x0400) {
        if (line[0] == '\n' || line[0] == ';' || line[0] == '#') continue;

        char op[16];
        int r1 = 0;
        int r2 = 0;
        int value = 0;
        uint16_t opcode = 0;
        uint16_t rs = 0;
        uint16_t rt = 0;
        uint16_t imm = 0;
        uint16_t instruction = 0;

        if (sscanf(line, "%s R%d R%d", op, &r1, &r2) == 3) {
            rs = (uint8_t)r1;
            rt = (uint8_t)r2;
            

            if      (strcmp(op, "ADD") == 0) opcode = 0;
            else if (strcmp(op, "SUB") == 0) opcode = 1;
            else if (strcmp(op, "MUL") == 0) opcode = 2;
            else if (strcmp(op, "EOR") == 0) opcode = 6;
            else if (strcmp(op, "BR")  == 0) opcode = 7;
            else {
                fprintf(stderr, "Unknown opcode: %s\n", op);
                exit(EXIT_FAILURE);
            }
            instruction = (opcode << 12) | ((rs & 0x3F) << 6) | (rt & 0x3F);
            printf("Loaded: %04X at addr %d from line: %s", instruction, addr, line);
            p->instr_mem[addr++] = instruction;

        } else if (sscanf(line, "%s R%d %d", op, &r1, &value) == 3) {
            rs = (uint8_t)r1;
            imm = (uint8_t)value;

            if      (strcmp(op, "MOVI") == 0) opcode = 3;
            else if (strcmp(op, "BEQZ") == 0) opcode = 4;
            else if (strcmp(op, "ANDI") == 0) opcode = 5;
            else if (strcmp(op, "SAL")  == 0) opcode = 8;
            else if (strcmp(op, "SAR")  == 0) opcode = 9;
            else if (strcmp(op, "LDR")  == 0) opcode = 10;
            else if (strcmp(op, "STR")  == 0) opcode = 11;
            else {
                fprintf(stderr, "Unknown opcode: %s\n", op);
                exit(EXIT_FAILURE);
            }

            if (opcode == 3 || opcode == 4 || opcode == 5 || opcode == 10 || opcode == 11 ||opcode == 8 ||opcode == 9 ) {
                instruction = (opcode << 12) | ((rs & 0x3F) << 6) | (imm & 0x3F);
            } else {
                instruction = (opcode << 12) | ((rs & 0x3F) << 6) | (rt & 0x3F);
            }
            printf("Loaded: %04X at addr %d from line: %s", instruction, addr, line);
            p->instr_mem[addr++] = instruction;

        } else {
            fprintf(stderr, "Invalid instruction format: %s", line);
            exit(EXIT_FAILURE);
        }
    }

    printf("\nLoaded %d instructions\n", addr - 0x020F);
    fclose(file);
}

uint8_t mem_read_data(Processor *p, uint16_t addr) {
    if (addr >= 2048) return 0;
    return p->data_mem[addr];
}

void mem_write_data(Processor *p, uint16_t addr, uint8_t data) {
    if (addr >= 2048) return;
    p->data_mem[addr] = data;
    printf("[EX] Memory[0x%04X] updated to 0x%02X\n", addr, data);
}

void mem_print_instr(const Processor *p) {
    printf("Instruction Memory:\n");
    for (int i = 0; i < 1024; i++) {
        if (p->instr_mem[i]) {
            uint16_t instruction = p->instr_mem[i];
            uint8_t opcode = (instruction >> 12) & 0x0F;
            uint8_t rs = (instruction >> 6) & 0x3F;
            uint8_t rt = instruction & 0x3F;
            printf("0x%04X: 0x%04X (opcode=%d, rs=%d, rt=%d)\n", i, instruction, opcode, rs, rt);
        }
    }
}

void mem_print_data(const Processor *p) {
    printf("Data Memory:\n");
    for (int i = 0; i < 2048; i++) {
        if (p->data_mem[i])
            printf("0x%04X: 0x%02X\n", i, p->data_mem[i]);
    }
}
