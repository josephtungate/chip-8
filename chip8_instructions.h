#ifndef CHIP8_INSTRUCTIONS_H_GUARD
#define CHIP8_INSTRUCTIONS_H_GUARD

#include "chip8_state.h"

void C8_0NNN(void);
void C8_00E0(C8_State *state);
void C8_00EE(C8_State *state);
void C8_1NNN(C8_State *state, uint16_t NNN);
void C8_2NNN(C8_State *state, uint16_t NNN);
void C8_3XNN(C8_State *state, uint8_t X, uint8_t NN);
void C8_4XNN(C8_State *state, uint8_t X, uint8_t NN);
void C8_5XY0(C8_State *state, uint8_t X, uint8_t Y);
void C8_6XNN(C8_State *state, uint8_t X, uint8_t NN);
void C8_7XNN(C8_State *state, uint8_t X, uint8_t NN);
void C8_8XY0(C8_State *state, uint8_t X, uint8_t Y);
void C8_8XY1(C8_State *state, uint8_t X, uint8_t Y);
void C8_8XY2(C8_State *state, uint8_t X, uint8_t Y);
void C8_8XY3(C8_State *state, uint8_t X, uint8_t Y);
void C8_8XY4(C8_State *state, uint8_t X, uint8_t Y);
void C8_8XY5(C8_State *state, uint8_t X, uint8_t Y);
void C8_8XY6(C8_State *state, uint8_t X, uint8_t Y);
void C8_8XY7(C8_State *state, uint8_t X, uint8_t Y);
void C8_8XYE(C8_State *state, uint8_t X, uint8_t Y);
void C8_9XY0(C8_State *state, uint8_t X, uint8_t Y);
void C8_ANNN(C8_State *state, uint16_t NNN);
void C8_BNNN(C8_State *state, uint16_t NNN);
void C8_CXNN(C8_State *state, uint8_t X, uint8_t NN);
void C8_DXYN(C8_State *state, uint8_t X, uint8_t Y, uint8_t N);
void C8_EX9E(C8_State *state, uint8_t X);
void C8_EXA1(C8_State *state, uint8_t X);
void C8_FX07(C8_State *state, uint8_t X);
void C8_FX0A(C8_State *state, uint8_t X);
void C8_FX15(C8_State *state, uint8_t X);
void C8_FX18(C8_State *state, uint8_t X);
void C8_FX1E(C8_State *state, uint8_t X);
void C8_FX29(C8_State* state, uint8_t X);
void C8_FX33(C8_State* state, uint8_t X);
void C8_FX55(C8_State *state, uint8_t X);
void C8_FX65(C8_State *state, uint8_t X);

#endif