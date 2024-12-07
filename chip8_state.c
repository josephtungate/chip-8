#include "chip8_state.h"
#include "chip8_error.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* Creates and initialises a C8_State according to given config.
 * Returns pointer to C8_State on success.
 * Returns NULL pointer on fail.
 * Returned C8_State should be freed with C8_DestroyState
 */
C8_State *C8_CreateState(C8_Config *config){
    if (config == NULL){
        C8_SetError("C8_CreateState received NULL argument for config.");
        return NULL;
    }

    C8_State *state = malloc(sizeof(C8_State));
    if (state == NULL){
        C8_SetError("C8_CreateState could not allocate memory for C8_State struct.");
        return NULL;
    }

    state->memory = malloc(sizeof(uint8_t) * config->memorySize);
    if (state->memory == NULL){
        C8_SetError("C8_CreateState could not allocate memory for CHIP_8 memory.");
        return NULL;
    }

    state->stack = malloc(sizeof(uint16_t) * config->stackSize);
    if (state->stack == NULL){
        C8_SetError("C8_CreateState could not allocate memory for stack.");
        return NULL;
    }

    state->display = malloc(sizeof(uint8_t) * config->displayHeight * config->displayWidth);
    if (state->display == NULL){
        C8_SetError("C8_CreateState could not allocate memory for display.");
        return NULL;
    }
    
    state->config = malloc(sizeof(C8_Config));
    if (state->config == NULL){
        C8_SetError("C8_CreateState could not allocate memory for config.");
        return NULL;
    }
    
    memset(state->memory, 0, sizeof(uint8_t) * config->memorySize);
    memset(state->stack, 0, sizeof(uint16_t) * config->stackSize);
    memset(state->display, 0, sizeof(uint8_t) * config->displayHeight * config->displayWidth);
    memcpy(state->config, config, sizeof(*config));

    state->pc = config->programAddress;
    state->i = 0;
    state->sp = 0;
    for (int i = 0; i < CHIP8_STATE_V_COUNT; i++) state->v[i] = 0;
    state->delayTimer = 0;
    state->soundTimer = 0;
    state->key = CHIP8_STATE_NULL_KEY;

    return state;
}

/* Frees the given C8_State. */
void C8_DestroyState(C8_State *state){
    free(state->memory);
    free(state->stack);
    free(state->display);
    free(state->config);
    free(state);
}