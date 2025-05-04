#include "processor.h"
#include <stdio.h>


static void update_flags(Processor *p, uint8_t result, uint8_t op1, uint8_t op2, Opcode op) {
    if (result == 0)       p->SREG |= FLAG_Z;  else p->SREG &= ~FLAG_Z;
    
    if (result & 0b10000000)     p->SREG |= FLAG_N;  else p->SREG &= ~FLAG_N;

    if (op == OP_ADD || op == OP_SUB) {
        uint16_t tmp = (op == OP_ADD) ? (op1 + op2) : ((int)op1 - (int)op2);
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


static void decode(Processor *p) {
  if (p->IF_ID.instr && !p->stall) {
      p->ID_EX = p->IF_ID;  

      p->ID_EX.opcode = (p->IF_ID.instr >> 12) & 0b1111;       
      p->ID_EX.r1     = (p->IF_ID.instr >> 6)  & 0b111111;     

      if (p->ID_EX.opcode <= OP_MUL
       || p->ID_EX.opcode == OP_EOR
       || p->ID_EX.opcode == OP_BR) {
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
    uint8_t  op2 = (p->ID_EX.opcode <= OP_MUL || p->ID_EX.opcode == OP_EOR || p->ID_EX.opcode == OP_BR)
                   ? p->R[r2] : (uint8_t)imm;
    uint8_t  result = 0;

    switch(p->ID_EX.opcode) {
      case OP_ADD:
        result = op1 + op2;
        p->R[r1] = result;
        update_flags(p, result, op1, op2, OP_ADD);
        break;

      case OP_SUB:
        result = op1 - op2;
        p->R[r1] = result;
        update_flags(p, result, op1, op2, OP_SUB);
        break;

      case OP_MUL:
        result = op1 * op2;
        p->R[r1] = result;
        update_flags(p, result, op1, op2, OP_MUL);
        break;

      case OP_MOVI:
        p->R[r1] = (uint8_t)imm;
        break;

      case OP_ANDI:
        result = op1 & op2;
        p->R[r1] = result;
        update_flags(p, result, op1, op2, OP_ANDI);
        break;

      case OP_EOR:
        result = op1 ^ op2;
        p->R[r1] = result;
        update_flags(p, result, op1, op2, OP_EOR);
        break;

      case OP_SAL:
        result = op1 << op2;
        p->R[r1] = result;
        update_flags(p, result, op1, op2, OP_SAL);
        break;

      case OP_SAR:
        result = (uint8_t)((int8_t)op1 >> op2);
        p->R[r1] = result;
        update_flags(p, result, op1, op2, OP_SAR);
        break;

      case OP_LDR:
        result = p->data_mem[(uint16_t)imm];
        p->R[r1] = result;
        break;

      case OP_STR:
        p->data_mem[(uint16_t)imm] = p->R[r1];
        break;

      case OP_BEQZ:
        if (p->R[r1] == 0) {
          p->PC    = p->ID_EX.pc + 1 + imm;
          p->stall = 2;       
        }
        break;

      case OP_BR: {
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