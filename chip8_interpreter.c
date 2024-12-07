#include "chip8_state.h"
#include "chip8_error.h"
#include "chip8_instructions.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>

int C8_LoadProgram(C8_State *state, char *path){
    FILE *file = fopen(path, "rb");
    if (file == NULL){
        C8_SetError("Could not open program file.");
        return 0;
    }

    uint16_t address = state->config->programAddress;
    int byte;
    while ((byte = getc(file)) != EOF){
        if (address >= state->config->memorySize){
            C8_SetError("Program is too big to load into memory.");
            return 0;
        }
        state->memory[address] = (uint8_t)byte;
        address++;
        printf("%u %x\n", address, byte);
    }

    fclose(file);
    return 1;
}

int C8_LoadFont(C8_State *state, uint8_t *font, uint8_t fontLen){
    memcpy(&(state->memory[state->config->fontAddress]), font, fontLen);
    return 1;
}

//Performs a single fetch decode execute cycle.
void C8_FDE(C8_State *state){
    //fetch - Instructions occupy 2 bytes, big endian style.
    uint16_t instruction;
    instruction = state->memory[state->pc];
    instruction = instruction << 8;
    instruction += state->memory[state->pc+1];
    state->pc += 2;

    //Decode - Split the instruction.
    uint8_t msnibble = instruction >> 12; 
    uint8_t xnibble = (instruction >> 8) & 0x0F;
    uint8_t ynibble = (instruction >> 4) & 0x0F;
    uint8_t nnibble = instruction & 0x0F;
    uint8_t nnbyte = instruction & 0xFF;
    uint16_t nnnaddress = instruction & 0x0FFF;

    //Execute - Switch to function representing instruction and run.
    switch (msnibble){
        case 0x0:
            switch (instruction){
                case 0x00E0:
                    C8_00E0(state); //clear screen.
                    break;
                case 0x00EE:
                    C8_00EE(state);
                    break;
                default:
                    C8_0NNN(); //does nothing.
                    break;
            }
            break;

        case 0x1:
            C8_1NNN(state, nnnaddress); //jump to NNN.
            break; 

        case 0x2:
            C8_2NNN(state, nnnaddress); //call subroutine NNN.
            break;

        case 0x3:
            C8_3XNN(state, xnibble, nnbyte); //skip next if vx == nn.
            break;

        case 0x4:
            C8_4XNN(state, xnibble, nnbyte); //skip next if vx != nn.
            break;

        case 0x5:
            C8_5XY0(state, xnibble, ynibble); //skip next if vx == vy.
            break;

        case 0x6:
            C8_6XNN(state, xnibble, nnbyte); //sets vx = nn.
            break;

        case 0x7:
            C8_7XNN(state, xnibble, nnbyte); //adds nn to vx w-out carry.
            break;

        case 0x8:
            switch (nnibble){
                case 0:
                    C8_8XY0(state, xnibble, ynibble); //vx = vy.
                    break;

                case 1:
                    C8_8XY1(state, xnibble, ynibble); //vx |= vy.
                    break;

                case 2:
                    C8_8XY2(state, xnibble, ynibble); //vx &= vy.
                    break;

                case 3:
                    C8_8XY3(state, xnibble, ynibble); //vx ^= vy.
                    break;

                case 4:
                    C8_8XY4(state, xnibble, ynibble); //vx += vy. vf = overflow.
                    break;

                case 5:
                    C8_8XY5(state, xnibble, ynibble); //vx -= vy. vf != overflow.
                    break;

                case 6:
                    //configurable behaviour. C8_CONFIG_SHIFT_VY.
                    C8_8XY6(state, xnibble, ynibble); //vx = vy >> 1. vf = overflow.
                    break;

                case 7:
                    C8_8XY7(state, xnibble, ynibble); //vx = vy - vx. vf != overflow.
                    break;

                case 0xE:
                    //configurable behaviour. C8_CONFIG_SHIFT_VY.
                    C8_8XYE(state, xnibble, ynibble); //vx = vy << 1. vf = overflow.
                    break;

                default:
                    #ifdef C8_WARNINGS
                        printf("warning: unrecognised 8-series instruction %x", instruction); 
                    #endif
                    break;
            }
            break;

        case 0x9:
            C8_9XY0(state, xnibble, ynibble); //skip next if vx != vy.
            break;

        case 0xA:
            C8_ANNN(state, nnnaddress); //sets I to NNN.
            break;

        case 0xB:
            //Configurable behaviour - C8_CONFIG_JUMP_XNN.
            C8_BNNN(state, nnnaddress); //jump with offset.
            break;

        case 0xC:
            C8_CXNN(state, xnibble, nnbyte);
            break;

        case 0xD:
            C8_DXYN(state, xnibble, ynibble, nnibble); //draw sprite to buffer.
            break;

        case 0xE:
            switch (nnbyte){
                case 0x9E:
                    C8_EX9E(state, xnibble);
                    break;
                case 0xA1:
                    C8_EXA1(state, xnibble);
                    break;
                default:
                #ifdef C8_WARNINGS
                    printf("Encountered unrecognised instruction %x at %x",
                    instruction, state->pc - 2);
                #endif
                    break;
            }
            break;

        case 0xF:
            switch (nnbyte){
                case 0x07:
                    C8_FX07(state, xnibble); //vx = delay timer.
                    break;

                case 0x0A:
                    C8_FX0A(state, xnibble); //get key (block).
                    break;
                
                case 0x15:
                    C8_FX15(state, xnibble); //delay timer = vx.
                    break;

                case 0x18:
                    C8_FX18(state, xnibble); //sound timer = vx.
                    break;

                case 0x1E:
                    C8_FX1E(state, xnibble); //i += vx.
                    break;

                case 0x29:
                    C8_FX29(state, xnibble); //i = loc of character vx.
                    break;

                case 0x33:
                    C8_FX33(state, xnibble); //store BCD of vx starting at i.
                    break;

                case 0x55:
                    C8_FX55(state, xnibble); //store registers v0 to vx in memory starting at i.
                    break;

                case 0x65:
                    C8_FX65(state, xnibble); //load registers v0 to vx in memory starting at i.
                    break;

                default:
                    #ifdef C8_WARNINGS
                        printf("Encountered unrecognised instruction %x at %x",
                        instruction, state->pc - 2);
                    #endif
                    break;
            }
            break;

        default:
            #ifdef C8_WARNINGS
                printf("Encountered unrecognised instruction %x at %x",
                 instruction, state->pc - 2);
            #endif
            break;
    }
}