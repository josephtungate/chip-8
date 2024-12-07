#include "chip8_instructions.h"
#include <stdlib.h>
#include <stdio.h>
/* IMPORTANT
 * Every instruction assumes that the PC has ALREADY BEEN INCREMENTED this cycle.
*/
//TODO: Ensure PC increments don't mess with function calls, returns, and jumps.
//TODO: Warnings when overflows etc.


//Convert x,y coordinate to index for C8_State display.
int cti(C8_State *state, int x, int y){
    return y * state->config->displayWidth + x;
}

//Execute machine code routine.
//Useless instruction in modern CHIP-8.
void C8_0NNN(){
    return;
}

//Clear screen.
void C8_00E0(C8_State *state){
    for (int i = 0; i < state->config->displayHeight * state->config->displayWidth; i++)
            state->display[i] = 0;
}

//Return from subroutine.
void C8_00EE(C8_State *state){
    state->sp--;
    state->pc = state->stack[state->sp];
}

//Jump to memory address NNN.
void C8_1NNN(C8_State *state, uint16_t NNN){
    state->pc = NNN;
}

//Subroutine at NNN.
void C8_2NNN(C8_State *state, uint16_t NNN){
    state->stack[state->sp] = state->pc;
    state->sp++;
    state->pc = NNN;

    #ifdef C8_WARNINGS
        if (state->sp > C8_STACK_SIZE)
            printf("warning: stack overflow.");
    #endif
}

//Skip next instruction if vx equals nn.
void C8_3XNN(C8_State *state, uint8_t X, uint8_t NN){
    if(state->v[X] == NN)
        state->pc += 2;
}

//Skip next instruction if vx is not equal to nn.
void C8_4XNN(C8_State *state, uint8_t X, uint8_t NN){
    if(state->v[X] != NN)
        state->pc += 2;
}

//Skip next instruction if vx equals vy.
void C8_5XY0(C8_State *state, uint8_t X, uint8_t Y){
    if (state->v[X] == state->v[Y])
        state->pc += 2;
}

//Set register X to value NN.
void C8_6XNN(C8_State *state, uint8_t X, uint8_t NN){
    state->v[X] = NN;
}

//Add value NN to register VX.
void C8_7XNN(C8_State *state, uint8_t X, uint8_t NN){
    state->v[X] += NN; //Overflowing is directly modelled by v[X] being uint8.
}

//Copy VY into vX.
void C8_8XY0(C8_State *state, uint8_t X, uint8_t Y){
    state->v[X] = state->v[Y];
}

//VX = VX binary OR VY.
void C8_8XY1(C8_State *state, uint8_t X, uint8_t Y){
    state->v[X] |= state->v[Y];
}

//VX &= VY.
void C8_8XY2(C8_State *state, uint8_t X, uint8_t Y){
    state->v[X] &= state->v[Y];
}

//VX ^= VY.
void C8_8XY3(C8_State *state, uint8_t X, uint8_t Y){
    state->v[X] ^= state->v[Y];
}

//VX = VX + VY. VF is set to 1 if overflow. Otherwise, VF = 0.
void C8_8XY4(C8_State *state, uint8_t X, uint8_t Y){
    uint16_t sum = state->v[X] + state->v[Y];
    state->v[X] = (uint8_t)sum & 0xFF;
    state->v[0xF] = (sum & 0xFF00) > 0;
}

//VX = VX - VY. VF is set to 1 if not underflow. Otherwise, VF = 0.
void C8_8XY5(C8_State *state, uint8_t X, uint8_t Y){
    int16_t difference = state->v[X] - state->v[Y];
    state->v[X] = (uint8_t)(difference & 0xFF);
    state->v[0xF] = difference >= 0;
}

//Shift VX right one. VF = overflow.
//Configurable behaviour - C8_CONFIG_INSTRUCTION_SHIFT_INPLACE 
void C8_8XY6(C8_State *state, uint8_t X, uint8_t Y){
    if (state->config->instructionMode & C8_CONFIG_INSTRUCTION_SHIFT_INPLACE)
        state->v[X] = state->v[Y];

    state->v[0xF] = state->v[X] & 0x01;
    state->v[X] = state->v[X] >> 1;
}

//VX = VY - VX. VF is set to 1 if not underflow. Otherwise, VF = 0.
void C8_8XY7(C8_State *state, uint8_t X, uint8_t Y){
    int16_t difference = state->v[Y] - state->v[X];
    state->v[X] = (uint8_t)(difference & 0xFF);
    state->v[0xF] = difference >= 0;
}

//Shift VX left one. VF = overflow.
//Configurable behaviour - C8_CONFIG_INSTRUCTION_SHIFT_INPLACE 
void C8_8XYE(C8_State *state, uint8_t X, uint8_t Y){
    if (state->config->instructionMode & C8_CONFIG_INSTRUCTION_SHIFT_INPLACE)
        state->v[X] = state->v[Y];

    state->v[0xF] = (state->v[X] & 0x80) > 0;
    state->v[X] = state->v[X] << 1;
}

//Skip next instruction if vx does not equal vy.
void C8_9XY0(C8_State *state, uint8_t X, uint8_t Y){
    if (state->v[X] != state->v[Y])
        state->pc += 2;
}

//Set index register I.
void C8_ANNN(C8_State *state, uint16_t NNN){
    state->i = NNN;
}

//Jump with offset to NNN. Offset is V0 or VX depending on config.
//Configurable behaviour - C8_CONFIG_INSTRUCTION_JUMP_VX_OFFSET
void C8_BNNN(C8_State *state, uint16_t NNN){
    if (state->config->instructionMode & C8_CONFIG_INSTRUCTION_JUMP_VX_OFFSET)
        state->pc = NNN + state->v[(NNN & 0xF00) >> 8];
    else
        state->pc = NNN + state->v[0];
}

//VX = random & NN.
void C8_CXNN(C8_State *state, uint8_t X, uint8_t NN){
    //TODO: allow for seed setting.
    uint16_t random = rand() % 256;
    state->v[X] = (uint8_t)random & NN;
}

//Draw.
//At (vx, vy), draw an N height sprite stored starting at location I.
void C8_DXYN(C8_State *state, uint8_t X, uint8_t Y, uint8_t N){
    //Starting coordinates wrap around screen.
    uint16_t xstart = state->v[X] % state->config->displayWidth;
    uint16_t ystart = state->v[Y] % state->config->displayHeight;

    state->v[0xF] = 0; //reset the overwrite flag.

    for (int spriteRow = 0; 
     spriteRow < N && spriteRow + ystart < state->config->displayHeight;
     spriteRow++){
        uint8_t spriteByte = state->memory[state->i + spriteRow];
        for (int spriteCol = 0;
         spriteCol < 8 && spriteCol + xstart < state->config->displayWidth;
         spriteCol++){
            uint8_t pixelValue = (spriteByte >> (7 - spriteCol)) & 0x1;
            state->v[0xF] |= state->display[cti(state, xstart + spriteCol, ystart + spriteRow)] 
                             & pixelValue;
            state->display[cti(state, xstart + spriteCol, ystart + spriteRow)] ^= pixelValue;
        }
    }
}

//Skip if current key press = vx.
//Configurable behaviour - C8_State.keyMode.
void C8_EX9E(C8_State *state, uint8_t X){
    //TODO: Should update key field?
    if (state->config->keyMode & C8_CONFIG_KEYPRESS_USE_GIVEN_KEY){
        if (state->v[X] == state->key)
            state->pc += 2;
    } else if (state->config->keyMode & C8_CONFIG_KEYPRESS_GET_KEY){
        if (state->v[X] == state->config->getKey())
            state->pc += 2;
    }

}

//Skip if current key press != vx.
//Configurable behaviour - C8_State.keyMode.
void C8_EXA1(C8_State *state, uint8_t X){
    //TODO: Should update key field?
    if (state->config->keyMode & C8_CONFIG_KEYPRESS_USE_GIVEN_KEY){
        if (state->v[X] != state->key)
            state->pc += 2;
    } else if (state->config->keyMode & C8_CONFIG_KEYPRESS_GET_KEY){
        if (state->v[X] != state->config->getKey())
            state->pc += 2;
    }
}

//VX = delay timer.
void C8_FX07(C8_State *state, uint8_t X){
    state->v[X] = state->delayTimer;
}

//delay timer = VX.
void C8_FX15(C8_State *state, uint8_t X){
    state->delayTimer = state->v[X];
}

//sound timer = VX.
void C8_FX18(C8_State *state, uint8_t X){
    state->soundTimer = state->v[X];
}

//i += VX. Overflow is configuarable.
//Configurable behaviour - C8_CONFIG_INSTRUCTION_FX1E_OVERFLOW.
void C8_FX1E(C8_State *state, uint8_t X){
    state->i += state->v[X];
    if (state->config->instructionMode & C8_CONFIG_INSTRUCTION_FX1E_OVERFLOW)
        state->v[0xF] = (state->i & 0xF000) > 0;
}

//Get key and store in VX. Blocking. User specifies actual key getting function.
void C8_FX0A(C8_State *state, uint8_t X){
    printf("here");
    if (state->config->instructionMode & C8_CONFIG_KEYPRESS_USE_GIVEN_KEY)
        state->v[X] = state->key;
    else if (state->config->instructionMode & C8_CONFIG_KEYPRESS_GET_KEY){
        state->pc -= 2;
        state->v[X] = state->config->getKeyBlocking();
        state->pc += 2;
    }
}

//I = address of character vX.
void C8_FX29(C8_State* state, uint8_t X){
    state->i = (state->v[X] & 0x0F) * 5 + state->config->fontAddress;
}

//BCD conversion.
void C8_FX33(C8_State* state, uint8_t X){
    uint8_t buffer = state->v[X];

    state->memory[state->i] = buffer / 100;
    buffer %= 100;

    state->memory[state->i + 1] = buffer / 10;
    buffer %= 10;

    state->memory[state->i + 2] = buffer;
}

//Write registers v0 to vx into memory starting at i.
void C8_FX55(C8_State *state, uint8_t X){
    uint8_t j;

    for (j = 0; j <= X; j++){
        state->memory[state->i + j] = state->v[j];
    }

    if (state->config->instructionMode & C8_CONFIG_INSTRUCTION_FX55_FX65_INC_I)
        state->i = j;
}

//Load registers v0 to vx from memory starting at i.
void C8_FX65(C8_State *state, uint8_t X){
    uint8_t j;

    for (j = 0; j <= X; j++){
        state->v[j] = state->memory[state->i + j];
    }

    if (state->config->instructionMode & C8_CONFIG_INSTRUCTION_FX55_FX65_INC_I)
        state->i = j;
}
