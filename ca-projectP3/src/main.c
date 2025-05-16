#include "processor.h"
#include <stdio.h>

int main() {
    Processor cpu;
    proc_init(&cpu);
    mem_init(&cpu); 
    mem_load_program(&cpu, "C:\\Users\\nourh\\OneDrive\\Documents\\GitHub\\ca-project\\ca-projectP3\\src\\program.txt");
    printf("Instruction memory loaded.\n");
    mem_print_instr(&cpu);

    printf("===== Simulation Start =====\n");

    bool isrunning = true;
    int cyclescounter = 0;

    while (isrunning) {
        processa_cycle(&cpu);
        if(!cpu.EX_valid && !cpu.IF_ID.valid && !cpu.ID_EX.valid && cpu.PC>=1024 ){
            break;
        }
        else{
              print_pipeline(&cpu, ++cyclescounter);
        }
        isrunning = cpu.IF_ID.valid || cpu.ID_EX.valid || cpu.EX_valid || cpu.PC < 1024;
    }

    printf("\n===== Final Registers =====\n");
    print_registers(&cpu);
    printf("PC: 0x%04X\n", cpu.PC);
    printf("SREG: 0x%02X\n", cpu.SREG);

    printf("\n===== Final Instruction Memory =====\n");
    mem_print_instr(&cpu);

    printf("\n===== Final Data Memory =====\n");
    mem_print_data(&cpu);

    return 0;
}
