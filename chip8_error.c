#include <string.h>
#include "chip8_error.h"

#define ERROR_STRING_LEN 256

static char error_string[ERROR_STRING_LEN] = "";

void C8_SetError(const char *string){
    strncpy(error_string, string, ERROR_STRING_LEN);
    error_string[ERROR_STRING_LEN - 1] = '\0';
}

const char *C8_GetError(){
    return error_string;
}

void C8_ClearError(){
    error_string[0] = '\0';
}