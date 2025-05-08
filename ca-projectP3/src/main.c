
#include "processor.h"
#include <stdio.h>

int main() {
    Processor cpu;
    proc_init(&cpu);
    mem_load_program(&cpu, "program.txt");

    printf("===== Simulation Start =====\n");
    while (cpu.PC < 1024 || cpu.IF_ID.valid || cpu.ID_EX.instr) {
        proc_cycle(&cpu);
        print_pipeline(&cpu);
    }

    printf("\n===== Final Registers =====\n");
    print_registers(&cpu);

    printf("\n===== Final Data Memory =====\n");
    mem_print_data(&cpu);
    return 0;
}