#include "processor.h"
#include <stdio.h>


static void update_flags(Processor *p, uint8_t result, uint8_t op1, uint8_t op2, uint8_t op) {
    if (result == 0)       p->SREG |= FLAG_Z;  else p->SREG &= ~FLAG_Z;
    
    if (result & 0b10000000)     p->SREG |= FLAG_N;  else p->SREG &= ~FLAG_N;

    if (op == 0b0000 || op == 0b0001) {
        uint16_t tmp = (op == 0b0000) ? (op1 + op2) : ((int)op1 - (int)op2);
        if (tmp & 0b100000000)    p->SREG |= FLAG_C;  else p->SREG &= ~FLAG_C;
        uint8_t ovf = ((op1 ^ result) & (op2 ^ result)) >> 7;
        if (ovf)            p->SREG |= FLAG_V;  else p->SREG &= ~FLAG_V;
        if ( ((p->SREG & FLAG_N) >> 1) ^ (p->SREG & FLAG_V) )
                            p->SREG |= FLAG_S;  else p->SREG &= ~FLAG_S;
    }
}

static void fetch(Processor *p) {
    if (p->PC < 1024 && !p->stall) {
        p->IF_ID.instr = p->instr_mem[p->PC];
        p->IF_ID.pc = p->PC;
        p->PC++;
    }
}

// has sth to do with pipeline???
static void decode(Processor *p) {
  if (p->IF_ID.instr && !p->stall) {
      p->ID_EX = p->IF_ID;  

      p->ID_EX.opcode = (p->IF_ID.instr >> 12) & 0b1111;       
      p->ID_EX.r1     = (p->IF_ID.instr >> 6)  & 0b111111;     

      if (p->ID_EX.opcode <= 0b0010
       || p->ID_EX.opcode == 0b0110
       || p->ID_EX.opcode ==0b0111) {
          p->ID_EX.r2 = p->IF_ID.instr & 0b111111;             
      } else {
          p->ID_EX.imm = p->IF_ID.instr & 0b111111;           
          if (p->ID_EX.imm & 0b100000)                         
              p->ID_EX.imm |= 0b11000000;                      
      }
  }
}


static void execute(Processor *p) {
    if (!p->ID_EX.instr) return;

    uint8_t  r1  = p->ID_EX.r1;
    uint8_t  r2  = p->ID_EX.r2;
    int8_t   imm = p->ID_EX.imm;
    uint8_t  op1 = p->R[r1];
    uint8_t  op2 = (p->ID_EX.opcode <= 0010 || p->ID_EX.opcode == 0110 || p->ID_EX.opcode == 0111)
                   ? p->R[r2] : (uint8_t)imm;
    uint8_t  result = 0;

    switch(p->ID_EX.opcode) {
      case 0b0000:
        result = op1 + op2;
        p->R[r1] = result;
        update_flags(p, result, op1, op2, 0b0000);
        break;

      case 0b0001:
        result = op1 - op2;
        p->R[r1] = result;
        update_flags(p, result, op1, op2, 0001);
        break;

      case 0b0010:
        result = op1 * op2;
        p->R[r1] = result;
        update_flags(p, result, op1, op2, 0010);
        break;

      case 0b0011:
        p->R[r1] = (uint8_t)imm;
        break;

      case 0b0101:
        result = op1 & op2;
        p->R[r1] = result;
        update_flags(p, result, op1, op2, 0101);
        break;

      case 0b0110:
        result = op1 ^ op2;
        p->R[r1] = result;
        update_flags(p, result, op1, op2, 0110);
        break;

      case 0b1000:
        result = op1 << op2;
        p->R[r1] = result;
        update_flags(p, result, op1, op2, 1000);
        break;

      case 0b1001:
        result = (uint8_t)((int8_t)op1 >> op2);
        p->R[r1] = result;
        update_flags(p, result, op1, op2, 1001);
        break;

      case 0b1010: // LDR
        result = mem_read_data(p, (uint16_t)imm);
        p->R[r1] = result;
        break;
      case 0b1011: // STR
        mem_write_data(p, (uint16_t)imm, p->R[r1]);
        break;
      case 0b0100:
        if (p->R[r1] == 0) {
          p->PC    = p->ID_EX.pc + 1 + imm;
          p->stall = 2;       
        }
        break;

      case 0b0111: {
        uint16_t addr = ((uint16_t)op1 << 8) | op2;
        p->PC    = addr;
        p->stall = 2;         
        break;
      }

    
        break;
    }
}

void proc_cycle(Processor *p) {
    if (p->stall > 0) {
        p->stall--;
        if (p->stall == 0) {
            memset(&p->IF_ID, 0, sizeof(PipelineReg));
            memset(&p->ID_EX, 0, sizeof(PipelineReg));
        }
        return;
    }

    execute(p);
    decode(p);
    fetch(p);
}

void print_pipeline(const Processor *p) {
    printf("PC: 0x%04X | IF: 0x%04X | ID: 0x%04X | EX: 0x%04X\n",
           p->PC, p->IF_ID.instr, p->ID_EX.instr, p->ID_EX.instr);
}

void print_registers(const Processor *p) {
    printf("Registers:\n");
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            int reg = i * 8 + j;
            printf("R%-2d: 0x%02X ", reg, p->R[reg]);
        }
        printf("\n");
    }
    printf("SREG: [%c%c%c%c%c]\n",
           (p->SREG & FLAG_C) ? 'C' : '-',
           (p->SREG & FLAG_V) ? 'V' : '-',
           (p->SREG & FLAG_N) ? 'N' : '-',
           (p->SREG & FLAG_S) ? 'S' : '-',
           (p->SREG & FLAG_Z) ? 'Z' : '-');
}