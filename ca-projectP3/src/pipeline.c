#include "processor.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


static void update_flags(Processor *p, uint8_t result, uint8_t op1, uint8_t op2, uint8_t op) {
  p->SREG = 0;
  if (result == 0) p->SREG |= FLAG_Z;
  if (result & 0x80) p->SREG |= FLAG_N;

  if (op == 0b0000 || op == 0b0001) {
      uint16_t temp = (op == 0b0000) ? (uint16_t)op1 + op2 : (uint16_t)((int8_t)op1 - (int8_t)op2);
      if (temp & 0x100) p->SREG |= FLAG_C;
      uint8_t ovf = ((op1 ^ result) & (op2 ^ result)) >> 7;
      if (ovf) p->SREG |= FLAG_V;
      if (((p->SREG & FLAG_N) >> 1) ^ (p->SREG & FLAG_V)) p->SREG |= FLAG_S;
  }
}
void fetch(Processor *p) {
  if (p->PC < 1024) {
      uint16_t instr = p->instr_mem[p->PC];
      if (instr == 0) {
          //p->IF_ID.valid = false;
          //p->ID_EX.valid = false;
          p->PC = 1024;
          return;
      }
      p->IF_ID.instr = instr;
      p->IF_ID.pc    = p->PC;
      p->IF_ID.valid = true;
      p->PC++;
  }
}

void decode(Processor *p) {
    if (!p->IF_ID.valid) return;
    uint16_t instr = p->IF_ID.instr;
    ID_EX_Reg E = {0};
    E.instr   = instr;
    E.pc      = p->IF_ID.pc;
    E.opcode  = (instr >> 12) & 0x0F;
    E.rs      = (instr >> 6) & 0x3F;
 
    if (E.opcode == 3 || E.opcode == 4 || E.opcode == 5 || E.opcode == 10 || E.opcode == 11 ||E.opcode == 8 ||E.opcode == 9) {
        E.imm = (int16_t)((int8_t)(instr & 0x3F));
        E.rt=0;
    } else {
        E.imm=0;
        E.rt = (int16_t)((int8_t)(instr & 0x3F));
    }
    E.valueRS = p->R[E.rs];
    E.valueRT = p->R[E.rt];
    E.valid   = true;
    p->ID_EX = E;
    p->IF_ID.valid = false;
}

void execute(Processor *p) {
  if (!p->ID_EX.valid) return;
  uint8_t opcode = p->ID_EX.opcode;
  uint8_t rs = p->ID_EX.rs;
  uint8_t rt = p->ID_EX.rt;
  int8_t imm = p->ID_EX.imm;
  uint8_t val1 = p->ID_EX.valueRS;
  uint8_t val2 = (opcode <= 0b0010 || opcode == 0b0110 || opcode == 0b0111) ? p->ID_EX.valueRT : imm;
  uint8_t result = 0;
  switch (opcode) {
      case 0b0000: result = val1 + val2; break;
      case 0b0001: result = val1 - val2; break;
      case 0b0010: result = val1 * val2; break;
      case 0b0011: result = imm; break;
      case 0b0101: result = val1 & val2; break;
      case 0b0110: result = val1 ^ val2; break;
      case 0b1000: result = val1 << val2; break;
      case 0b1001: result = ((int8_t)val1) >> val2; break;
      case 0b1010: result = mem_read_data(p, imm); break;
      case 0b1011: mem_write_data(p, imm, p->R[rs]); goto skip_write;
      case 0b0100:
          if (p->R[rs] == 0) {
              p->PC = p->ID_EX.pc + 1 + imm;
              p->IF_ID.valid = false;
              p->ID_EX.valid = false;
              return;
          }
          goto skip_write;
      case 0b0111:
          p->PC = ((uint16_t)p->R[rs] << 8) | p->R[rt];
          p->IF_ID.valid = false;
          p->ID_EX.valid = false;
          return;
      default: break;
  }
  if (rs != 0) {
      p->R[rs] = result;
  }
  update_flags(p, result, val1, val2, opcode);
skip_write:
  p->ID_EX.valid = false;
}

void proc_cycle(Processor *p) {
   
    p->EX_instr = p->ID_EX.instr;
    p->EX_pc    = p->ID_EX.pc;
    p->EX_valid = p->ID_EX.valid;
    execute(p);


    decode(p);

 
    if (p->PC < 1024) {
        fetch(p);

    } 
    
}







void print_registers(const Processor *p) {
    printf("Registers:\n");
    int any = 0;
    for (int i = 0; i < 64; i++) {
        printf("R%02d: 0x%02X  ", i, p->R[i]);
        if (p->R[i] && i != 0) any = 1;
        if ((i & 7) == 7) printf("\n");
    }
    if (!any) printf("(all zero except R0)\n");
    printf("SREG: [%c%c%c%c%c]\n",
           (p->SREG & FLAG_C) ? 'C' : '-',
           (p->SREG & FLAG_V) ? 'V' : '-',
           (p->SREG & FLAG_N) ? 'N' : '-',
           (p->SREG & FLAG_S) ? 'S' : '-',
           (p->SREG & FLAG_Z) ? 'Z' : '-');
}

void print_pipeline(const Processor *p, int cycle) {
    static char if_buf[64], id_buf[128], ex_buf[128];
    printf("Clock Cycle %d\n", cycle);
   
    if (p->IF_ID.valid)
        sprintf(if_buf, "Instruction %d (PC=%d)", p->IF_ID.pc + 1, p->IF_ID.pc);
    else
        sprintf(if_buf, "-");
 
    if (p->ID_EX.valid){
        if ( p->ID_EX.opcode == 3 ||  p->ID_EX.opcode == 4 ||  p->ID_EX.opcode == 5 ||  p->ID_EX.opcode == 10 ||  p->ID_EX.opcode == 11 || p->ID_EX.opcode == 8 || p->ID_EX.opcode == 9) {
          sprintf(id_buf, "Instruction %d (opcode=%d, rs=R%d=%d, , imm=%d)", 
            p->ID_EX.pc + 1, p->ID_EX.opcode, p->ID_EX.rs, p->ID_EX.valueRS, p->ID_EX.imm);
    } else {
         sprintf(id_buf, "Instruction %d (opcode=%d, rs=R%d=%d, rt=R%d=%d)", 
            p->ID_EX.pc + 1, p->ID_EX.opcode, p->ID_EX.rs, p->ID_EX.valueRS, p->ID_EX.rt, p->ID_EX.valueRT);
        
            }
        }
    else
        sprintf(id_buf, "-");
  
    if (p->EX_valid)
        sprintf(ex_buf, "Instruction %d (PC=%d)", p->EX_pc + 1, p->EX_pc);
    else
        sprintf(ex_buf, "-");
    printf("| %-30s | %-60s | %-30s |\n", if_buf, id_buf, ex_buf);
}

