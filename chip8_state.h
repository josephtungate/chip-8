#ifndef CHIP8_STATE_H_GUARD
#define CHIP8_STATE_H_GUARD

#include <stdint.h>

#define C8_MEMORY_SIZE_STANDARD 4096
#define C8_STACK_SIZE_STANDARD 64
#define C8_DISPLAY_W_STANDARD 64
#define C8_DISPLAY_H_STANDARD 32
#define C8_FONT_ADDRESS_STANDARD 0x50
#define C8_PROGRAM_ADDRESS_STANDARD 0x200
#define C8_INSTRUCTION_MODE_STANDARD 0x4
static const uint8_t C8_FONT_STANDARD[] = {
0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
0x20, 0x60, 0x20, 0x20, 0x70, // 1
0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
0x90, 0x90, 0xF0, 0x10, 0x10, // 4
0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
0xF0, 0x10, 0x20, 0x40, 0x40, // 7
0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
0xF0, 0x90, 0xF0, 0x90, 0x90, // A
0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
0xF0, 0x80, 0x80, 0x80, 0xF0, // C
0xE0, 0x90, 0x90, 0x90, 0xE0, // D
0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};
#define C8_FONT_STANDARD_LEN 80
#define C8_DISPLAY_W_SUPERCHIP 128
#define C8_DISPLAY_H_SUPERCHIP 64

#define CHIP8_STATE_V_COUNT 16
#define CHIP8_STATE_NULL_KEY 0xFF

//C8_Config instructionMode flags
/* If set, 8XY6 and 8XYE shift VX WITHOUT copying VY to VX.
 * Otherwise, VY is copied to VX before shifting. */
#define C8_CONFIG_INSTRUCTION_SHIFT_INPLACE 0x1
/* If set, BNNN will behave as BXNN: jump to XNN + VX.
 * Otherwise, BNNN behaves normally and jumps to NNN + V0.*/
#define C8_CONFIG_INSTRUCTION_JUMP_VX_OFFSET 0x2
/* If set, FX1E will set VF to 1 if the addition
 * causes i >= 0x1000 (i.e. i goes outside 12-bit address range.)*/
#define C8_CONFIG_INSTRUCTION_FX1E_OVERFLOW 0x4
/* If set, FX55 and FX65 will increment i as they work.
 * otherwise, i is not changed. */
#define C8_CONFIG_INSTRUCTION_FX55_FX65_INC_I 0x8
/* If C8_Config keymode is set to this, requests for key presses
 * will use value of parent C8_State field */  
#define C8_CONFIG_KEYPRESS_USE_GIVEN_KEY 0x1
/* If C8_Config keymode is set to this, requests for key presses
 * will use values returned by parent C8_State getKey or getKeyBlocking functions. */
#define C8_CONFIG_KEYPRESS_GET_KEY 0x2

typedef struct C8_Config{
    uint16_t memorySize; //Size of CHIP-8 system's memory in bytes.
    uint8_t stackSize; //Size of CHIP-8 system's stack in bytes.
    uint16_t displayHeight; //Height of display in pixels.
    uint16_t displayWidth; //Width of display in pixels.
    uint16_t fontAddress; //Start of font sprites.
    uint16_t programAddress; //Start of program.
    uint8_t keyMode; //Flag determining how input is handled.
    uint8_t (*getKey)(); //A function for getting a key value.
    uint8_t (*getKeyBlocking)(); //A function for getting a key value and blocking.
    uint16_t timerClock; //Timers will be decremented every timerClock cycles. If 0, never decremented.
    uint8_t instructionMode; //Flags for ambiguous instructions.
} C8_Config;

/* Contains the state of a C8 system, including:
 * registers
 * stack
 * timers
 * display
 * memory
 * inputs
 * system configuation
 */
typedef struct C8_State{
    uint8_t *memory; //memory.
    uint16_t pc; //program counter.
    uint16_t i; //index register.
    uint16_t *stack; //stack.
    uint8_t sp; //stack pointer.
    uint8_t v[CHIP8_STATE_V_COUNT]; //16 general purpose variable registers.
    uint8_t delayTimer; //delay timer - behaviour configurable.
    uint8_t soundTimer; //sound timer - behaviour configurable.
    uint8_t *display; //Frame buffer.
    uint8_t key; //numeric value of currently pressed key.
    C8_Config *config; //State's configuration.
} C8_State;


C8_State *C8_CreateState(C8_Config *config);
void C8_DestroyState(C8_State *state);
#endif