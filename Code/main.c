#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "circularBuffer.h"

/*	[Hint1:] Before pushing or popping data, always ensure that
			the buffer is not full and not empty, respectively,
			to avoid data loss or invalid operations.

	[Hint2:] While it is possible to directly read from the file
			to the circular buffer, the logic is more complicated.
			The easiest version is to use a secondary linear buffer, with the same
			size, to where do read operations.*/

#define MAX_LINE 1024
#define MAX_ARGS 64
#define BUFFER_CHUNK_SIZE 1024

// void remove_newline(char *str);

// leer una linea completa
// int read_line_buffered(char *dest, int max_len);

int main() {
	CircularBuffer *buffer;
//    char mode_line[MAX_LINE];
//    char cmd_line1[MAX_LINE];
//    char cmd_line2[MAX_LINE];
//    char *args1[MAX_ARGS];
//    char *args2[MAX_ARGS];

	// initialize_buffer MAX_SIZE;

    // Bucle principal consumiendo el flujo controlado por el Buffer Circular

    //while (read_line_buffered(mode_line, sizeof(mode_line)) > 0)
	while (true)	
	{
        // remove_newline(mode_line);

        if (strcmp(mode_line, "SINGLE") == 0) {
			printf("SINGLE\n");
			// read the command
			// split command
			// execute for single (new function)
        }
        else if (strcmp(mode_line, "PIPED") == 0) {
			printf("PIPED\n");
			// read the 2 commands
			// split the commands
			// executed for piped (new funtion)
        }
        else if (strcmp(mode_line, "CONCURRENT") == 0) {
			printf("CONCURRENT\n");
			// read command
			// split the command innto tokens
			// execute for concurrent (new function) 
        }
        
		if (strcmp(mode_line, "EXIT") == 0)
            break;
    }
	// free memmory
	// buffer_deallocate(&cb);

    return (0);
}
