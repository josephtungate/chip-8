sdl_includes := C:\Users\Joe\programming\chip-8\x86_64-w64-mingw32\include
sdl_lib := C:\Users\Joe\programming\chip-8\x86_64-w64-mingw32\lib
all_o := application.o chip8_state.o chip8_interpreter.o chip8_instructions.o chip8_error.o 

debug.exe: $(all_o)
	gcc $(all_o) -L$(sdl_lib) -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -o debug

clear:
	del $(all_o)

chip8_state.o: chip8_state.c 
	gcc -c chip8_state.c -Wall 
	
chip8_interpreter.o: chip8_interpreter.c 
	gcc -c chip8_interpreter.c -Wall
	
chip8_instructions.o: chip8_instructions.c 
	gcc -c chip8_instructions.c -Wall 
	
chip8_error.o: chip8_error.c
	gcc -c chip8_error.c -Wall 
	
application.o: application.c
	gcc -c application.c -I$(sdl_includes) -Wall