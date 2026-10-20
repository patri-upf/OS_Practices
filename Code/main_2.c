#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include "circularBuffer.h"

#define MAX_COMMAND 60
#define MAX_ARGS    512
#define MAX_SIZE    2052

char    *read_line(CircularBuffer *circular_buff, int *reachedEOF)
{
    unsigned char    lineal_buff[MAX_SIZE];
    char    *line;
    int     size_element;
    int     free_space;
    ssize_t bytes_read;

    while(68)
    {
        size_element = buffer_size_next_element(circular_buff, '\n', *reachedEOF);

        if (size_element != -1)
        {
            line = malloc(size_element + 1);
            if (line == NULL)
                return NULL;

            for (int i=0; i<size_element; i++)
                line[i] = buffer_pop(circular_buff);
            
            if (line[size_element - 1] == '\n')
                size_element--;

            line[size_element] = '\0';

            return line;
        }

        if (*reachedEOF)
            return NULL;
        
        free_space = buffer_free_bytes(circular_buff);
        if (free_space == 0)
        {
            printf("Line too long\n");
            return NULL;
        }

        bytes_read = read(STDIN_FILENO, lineal_buff, free_space);

        if (bytes_read <= 0)
            *reachedEOF = 1;
        else
            for (int i=0; i<bytes_read; i++)
                buffer_push(circular_buff, lineal_buff[i]);
    }
}

int main( void )
{
    CircularBuffer circular_buff;
    char    *line;
    char    command_1[MAX_COMMAND];
    char    command_2[MAX_COMMAND];
    char    *args_1[MAX_ARGS];
    char    *args_2[MAX_ARGS];
    int     reachedEOF = 0;
    

    if (buffer_init(&circular_buff, MAX_SIZE))
        return EXIT_FAILURE;

    while ((line = read_line(&circular_buff, &reachedEOF)) != NULL)
    {

        if (bytes_read <= 0)
            reachedEOF = 1;

        // now we save in our principal circular buffer from secondary linear
        for (int i=0; i<bytes_read; i++)
            buffer_push(&circular_buff, lineal_buff[i]);

        int size_element = buffer_size_next_element(&circular_buff, '\n', reachedEOF);

        // read from circular buffer 
        for (int i=0; i<size_element - 1; i++)
            line[i] = buffer_pop(&circular_buff);
        */
        if (strcmp(line, "SINGLE") == 0)
        {
            //args1 = split_command();

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
            printf("BYE :)\n\tSee you soon...\n");
            break ;
        }
        else
        {
            printf("USAGE: \n");
        }
    }
}
