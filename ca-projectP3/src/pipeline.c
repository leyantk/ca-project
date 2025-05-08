#include "processor.h"
#include <stdio.h>


static void update_flags(Processor *p, uint8_t result,
  uint8_t op1, uint8_t op2, uint8_t op) {
// Zero flag
if (result == 0) p->SREG |= FLAG_Z; else p->SREG &= ~FLAG_Z;
// Negative flag
if (result & 0x80) p->SREG |= FLAG_N; else p->SREG &= ~FLAG_N;

// Carry, Overflow, Sign only for ADD/SUB
if (op == 0b0000 || op == 0b0001) {
uint16_t tmp = (op == 0b0000) ? (uint16_t)op1 + op2
               : (uint16_t)((int)op1 - (int)op2);
if (tmp & 0x100) p->SREG |= FLAG_C; else p->SREG &= ~FLAG_C;
uint8_t ovf = ((op1 ^ result) & (op2 ^ result)) >> 7;
if (ovf) p->SREG |= FLAG_V; else p->SREG &= ~FLAG_V;
if (((p->SREG & FLAG_N) >> 1) ^ (p->SREG & FLAG_V))
p->SREG |= FLAG_S;
else p->SREG &= ~FLAG_S;
}
}

static void fetch(Processor *p) {
if (p->PC < 1024 && p->stall == 0) {
p->IF_ID.instr = p->instr_mem[p->PC];
p->IF_ID.pc    = p->PC;
p->IF_ID.valid = true;
p->PC++;
}
}

static void decode(Processor *p) {
if (!p->IF_ID.valid || p->stall > 0) return;

uint16_t instr = p->IF_ID.instr;
ID_EX_Reg E;

E.instr   = instr;
E.pc      = p->IF_ID.pc;
E.opcode  = (instr >> 12) & 0x0F;
E.rs      = (instr >>  6) & 0x3F;
E.rt      =  instr        & 0x3F;
E.imm     = (int16_t)(E.rt << 10) >> 10;
E.address =  instr        & 0x0FFF;
E.rd      = (instr >>  2) & 0x0F;
E.shamt   =  instr        & 0x3F;
E.funct   =  instr        & 0x0F;

E.valueRS = p->R[E.rs];
E.valueRT = p->R[E.rt];

p->ID_EX = E;
p->IF_ID.valid = false;
}

static void execute(Processor *p) {
if (p->ID_EX.instr == 0) return;

uint8_t  rs   = p->ID_EX.rs;
uint8_t  rt   = p->ID_EX.rt;
int8_t   imm  = p->ID_EX.imm;
uint8_t  op1  = p->R[rs];
uint8_t  op2  = (p->ID_EX.opcode <= 0b0010 ||
p->ID_EX.opcode == 0b0110 ||
p->ID_EX.opcode == 0b0111)
? p->R[rt] : (uint8_t)imm;
uint8_t  result = 0;

switch (p->ID_EX.opcode) {
case 0b0000: // ADD
result = op1 + op2;
p->R[rs] = result;
update_flags(p, result, op1, op2, 0b0000);
break;
case 0b0001: // SUB
result = op1 - op2;
p->R[rs] = result;
update_flags(p, result, op1, op2, 0b0001);
break;
case 0b0010: // MUL
result = op1 * op2;
p->R[rs] = result;
update_flags(p, result, op1, op2, 0b0010);
break;
case 0b0011: // MOVI
p->R[rs] = (uint8_t)imm;
break;
case 0b0101: // ANDI
result = op1 & op2;
p->R[rs] = result;
update_flags(p, result, op1, op2, 0b0101);
break;
case 0b0110: // EOR
result = op1 ^ op2;
p->R[rs] = result;
update_flags(p, result, op1, op2, 0b0110);
break;
case 0b1000: // SAL
result = op1 << op2;
p->R[rs] = result;
update_flags(p, result, op1, op2, 0b1000);
break;
case 0b1001: // SAR
result = ((int8_t)op1) >> op2;
p->R[rs] = result;
update_flags(p, result, op1, op2, 0b1001);
break;
case 0b1010: // LDR
result = mem_read_data(p, (uint16_t)imm);
p->R[rs] = result;
break;
case 0b1011: // STR
mem_write_data(p, (uint16_t)imm, p->R[rs]);
break;
case 0b0100: // BEQZ
if (p->R[rs] == 0) {
p->PC = p->ID_EX.pc + 1 + imm;
p->stall = 2;
}
break;
case 0b0111: // BR
{
uint16_t addr = ((uint16_t)op1 << 8) | op2;
p->PC    = addr;
p->stall = 2;
}
break;
default:
break;
}

p->ID_EX.instr = 0;
}

void proc_cycle(Processor *p) {
if (p->stall > 0) {
p->stall--;
if (p->stall == 0) {
memset(&p->IF_ID, 0, sizeof p->IF_ID);
p->ID_EX.instr = 0;
}
return;
}

execute(p);
decode(p);
fetch(p);
}

void print_pipeline(const Processor *p) {
printf("PC: 0x%04X | IF: %s%04X | ID/EX: %04X\n",
p->PC,
p->IF_ID.valid ? "" : "--",
p->IF_ID.valid ? p->IF_ID.instr : 0,
p->ID_EX.instr);
}

void print_registers(const Processor *p) {
printf("Registers:\n");
for (int i = 0; i < 64; i++) {
printf("R%02d: 0x%02X  ", i, p->R[i]);
if ((i & 7) == 7) printf("\n");
}
printf("SREG: [%c%c%c%c%c]\n",
(p->SREG & FLAG_C) ? 'C' : '-',
(p->SREG & FLAG_V) ? 'V' : '-',
(p->SREG & FLAG_N) ? 'N' : '-',
(p->SREG & FLAG_S) ? 'S' : '-',
(p->SREG & FLAG_Z) ? 'Z' : '-');
}
