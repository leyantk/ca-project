// Processor state
typedef struct {
    uint8_t registers[64];      // R0-R63 (8-bit each)
    uint8_t sreg;               // Status register (flags: C,V,N,S,Z)
    uint16_t pc;                // 16-bit program counter
    uint16_t instruction_mem[1024]; // 16-bit words (Harvard)
    uint8_t data_mem[2048];     // 8-bit words (Harvard)
    
    // Pipeline registers (IF/ID, ID/EX)
    struct {
        uint16_t instruction;
        uint16_t pc_value;
        uint8_t r1, r2, imm;    // Decoded fields
    } if_id, id_ex;
} Processor;

// Function prototypes
void init_processor(Processor *p);
void load_program(Processor *p, const char *filename);
void execute_cycle(Processor *p);
void print_pipeline_status(const Processor *p);