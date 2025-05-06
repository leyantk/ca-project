#include "processor.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void mem_init(Processor *p) {
    memset(p->instr_mem, 0, sizeof(p->instr_mem));
    memset(p->data_mem, 0, sizeof(p->data_mem));
}


void mem_load_program(Processor *p, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) { perror("fopen"); exit(EXIT_FAILURE); }

    char line[256];
    uint16_t addr = 0;

    while (fgets(line, sizeof(line), file) && addr < 1024) {
        if (line[0] == ' '|| line[0] == ';') continue;
        bool flag=true;

        uint16_t instr = 0;
        uint16_t op , rs , rt ,  imm ;
        unsigned r1, r2, immediate;
        char label[64];

        // Initialize all fields to zero
        op = rs = rt = imm = 0;

        // Decode each instruction, fill all fields (unused remain 0)
        if (sscanf(line, "ADD R%u R%u", &r1, &r2) == 2) {
            op    = 0b0000;
            rs    = r1;
            rt    = r2;
            imm=0;
            flag=false;

        }
        else if (sscanf(line, "SUB R%u R%u", &r1, &r2) == 2) {
            op    = 0b0001;
            rs    = r1;
            rt    = r2;
            imm=0;
            flag=false;

        }
        else if (sscanf(line, "MUL R%u R%u", &r1, &r2) == 2) {
            op    = 0b0010;
            rs    = r1;
            rt    = r2;
            imm=0;
            flag=false;

        }
        else if (sscanf(line, "EOR R%u R%u", &r1, &r2) == 2) {
            op    = 0b0110;
            rs    = r1;
            rt    = r2;
            imm=0;
            flag=false;

        }
        else if (sscanf(line, "BR R%u R%u", &r1, &r2) == 2) {
            op    = 0b0111;
            rs    = r1;
            rt    = r2;
            imm=0;  
            flag=false;

        }
        else if (sscanf(line, "MOVI R%u %u", &r1, &immediate) == 2) {
            op    = 0b0011;
            rs    = r1;
            rt=0;
            imm   = immediate;
            
        }
        else if (sscanf(line, "ANDI R%u %u", &r1, &immediate) == 2) {
            op    = 0b0101;
            rs    = r1;
            rt=0;
            imm   = immediate;
           
        }
        else if (sscanf(line, "SAL R%u %u", &r1, &immediate) == 2) {
            op    = 0b1000;
            rs    = r1;
            rt=0;
            imm   = immediate;
        }
        else if (sscanf(line, "SAR R%u %u", &r1, &immediate) == 2) {
            op    = 0b1001;
            rs    = r1;
            rt=0;
            imm   = immediate;
        
        }
        else if (sscanf(line, "BEQZ R%u %u", &r1, &immediate) == 2) {
            op    = 0b0100;
            rs    = r1;
            rt    = 0;
            imm   = immediate;
        
        }
        else if (sscanf(line, "LDR R%u , %63s", &r1, &label) == 2) {
            op    = 0b1010;
            rs    = r1;
            rt=0;

            imm   =  label  ;
           
        }
        else if (sscanf(line, "STR R%u , %63s", &r1, &label) == 2) {
            op    = 0b1011;
            rs    = r1;
            rt=0;
            imm   =  label ;
         
        }
        else {
            fprintf(stderr, "Unknown instruction: %s", line);
            exit(EXIT_FAILURE);
        }

        if (!flag){
        instr = (uint16_t)((op << 12) | ((rs & 0x3F) << 6) | (rt & 0x3F));
        }
        else{
            instr=(uint16_t)((op << 12) | ((rs & 0x3F) << 6) | (imm & 0x3F));
        }

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
        if (p->instr_mem[i] != 0) {
            printf("0x%04X: 0x%04X\n", i, p->instr_mem[i]);
        }
    }
}


void mem_print_data(const Processor *p) {
    printf("Data Memory:\n");
    for (int i = 0; i < 2048; i++) {
        if (p->data_mem[i] != 0) {
            printf("0x%04X: 0x%02X\n", i, p->data_mem[i]);
        }
    }
}