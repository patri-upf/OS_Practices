#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include "circularBuffer.h"

#define MAX_SIZE    2052


int main( void )
{
    CircularBuffer circular_buff;
    char    lineal_buff[MAX_SIZE];
    char    line[MAX_SIZE];
    char    comman_1[MAX_COMMAND];
    char    comman_2[MAX_COMMAND];
    char    ags_1[MAX_ARGS];
    char    ags_2[MAX_ARGS];
    int     reachedEOF = 0;
    

    if (buffer_init(&circular_buff, MAX_SIZE))
        return EXIT_FAILURE;

    while (68)
    {
        // here we have the available space in our circular buffer
        int free_space = buffer_free_bytes(&circular_buff);

        // check if we have space in order to continue reading or not...
        if (free_space == 0)
            continue ;

        // reads from STDIN at most the free space 
        ssize_t bytes_read = read(STDIN_FILENO, lineal_buff, free_space);

        if (bytes_read <= 0)
            reachedEOF = 1;

        // now we save in our principal circular buffer from secondary linear
        for (int i=0; i<bytes_read; i++)
            buffer_push(&circular_buff, lineal_buff[i]);

        int size_element = buffer_size_next_element(&circular_buff, '\n', reachedEOF);

        for (int i=0; i<size_element - 1; i++)
            line[i] = buffer_pop(&circular_buff);

        if (strcmp(line, "SINGLE") == 0)
        {
            command_1 = 

            // execute_single();
        }
        else if (strcmp(line, "PIPED") == 0)
        {
            // execute_single();
        }
        else if(strcmp(line, "CONCURRENT") == 0)
        {
            // execute_single();
        }
        else if(strcmp(line, "EXIT") == 0)
        {
            printf("BYE :)\n\tSEe you soon...\n");
            break ;
        }
        else
        {
            printf("USAGE: "\n");
        }
    }
}
