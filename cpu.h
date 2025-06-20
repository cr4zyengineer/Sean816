/*
 * Headers
 *
 */
#include <stdint.h>
#include <stdbool.h>

/*
 * Definitions
 *
 */
typedef struct {
    // Special purpose registers
    uint16_t pc;
    uint16_t bp;
    uint16_t sp;

    // Memory full addressing registers for low byte and high byte equalling a full address
    // NOTE: As the cpu is 8bit we need 2 registers to have arbitary 16bit memory reading and writing without editing exectable memory, as I later will add MMU(I already wrote one)
    uint8_t ml;
    uint8_t mh;

    // Reserved for the instructions
    // NOTE: ta,tc,tc,td is created to minimise the argument bloat, and cmp is the result of cmp for JE and JNE to use
    // Updated instruction argument gathering system
    uint8_t instruction;
    bool operandsig[3];
    uint8_t *targ[3];
    uint8_t timm[3];
    uint8_t cmp;

    // Registers for a function to return a value       Return Register
    uint8_t ra;
    uint8_t rb;
    uint8_t rc;
    uint8_t rd;
    uint8_t re;
    uint8_t rf;
    uint8_t rg;
    uint8_t rh;

    // Registers that never change across calls         Global Register
    uint8_t ga;
    uint8_t gb;
    uint8_t gc;
    uint8_t gd;
    uint8_t ge;
    uint8_t gf;
    uint8_t gg;
    uint8_t gh;

    // Accumulator                                      General Purpose Register
    uint8_t a;

    // General purpose registers
    uint8_t b;
    uint8_t c;
    uint8_t d;
    uint8_t e;
    uint8_t f;
    uint8_t g;
    uint8_t h;

    // Register array for offsetting
    uint8_t *regtable[0xFF];
} cpu_core_t;

/*
 * Opcodes
 */
enum cpu_opcodes {
    // Execution
    OP_HLT = 0x00,

    // Data
    OP_LOAD = 0x01,
    OP_STORE = 0x02,
    OP_MHML = 0x03,
    OP_LOADLH = 0x04,
    OP_STORELH = 0x05,
    OP_MOV = 0x06,

    // Arithmetic
    OP_ADD = 0x07,
    OP_SUB = 0x08,
    OP_MUL = 0x09,
    OP_DIV = 0x0A,
    OP_INC = 0x0B,
    OP_DEC = 0x0C,

    // Flow control
    OP_JMP = 0x0D,
    OP_CMP = 0x0E,
    OP_JE = 0x0F,
    OP_JNE = 0x10,
    OP_JG = 0x11,
    OP_JL = 0x12,

    // Stack
    OP_PUSH = 0x13,
    OP_POP = 0x14,
    OP_CALL = 0x15,
    OP_RET = 0x16,

    // BIT Manipulation
    OP_AND = 0x17,
    OP_OR = 0x18,
    OP_XOR = 0x19,
    OP_NOT = 0x1A
};

typedef void (*instruction_t)(cpu_core_t*);

/*
 * Functions
 *
 */
cpu_core_t* cpu_create_core(void);
void cpu_exec_core(cpu_core_t *core);
void cpu_core_get_args(cpu_core_t *core, uint8_t count);
