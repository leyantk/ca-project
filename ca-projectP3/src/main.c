#include "processor.h"
#include <stdio.h>

int main() {
    Processor cpu;
    proc_init(&cpu);
    mem_init(&cpu);  // Initialize memory
    mem_load_program(&cpu, "C:\\Users\\Salma\\OneDrive\\Documents\\GitHub\\ca-project\\ca-projectP3\\src\\program.txt");
    printf("Instruction memory loaded.\n");
    mem_print_instr(&cpu);

    printf("===== Simulation Start =====\n");

    int max_cycles = 1024;  // to prevent infinite loops in edge cases
    int cycles = 0;
    bool running = true;

    while (running && cycles++ < max_cycles) {
        proc_cycle(&cpu);
        print_pipeline(&cpu);

        running = cpu.IF_ID.valid || cpu.ID_EX.valid || cpu.PC < 1024;
    }

    printf("\n===== Final Registers =====\n");
    print_registers(&cpu);

    printf("\n===== Final Data Memory =====\n");
    mem_print_data(&cpu);

    return 0;
}
