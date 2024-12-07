#ifndef CHIP8_INTERPRETER_H_GUARD
#define CHIP8_INTERPRETER_H_GUARD
#include "chip8_state.h"

int C8_LoadProgram(C8_State *state, char *path);
int C8_LoadFont(C8_State *state, uint8_t *font, uint8_t fontLen);
void C8_FDE(C8_State *state);

#endif