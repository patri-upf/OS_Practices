#include "utils.h"
#include "circularBuffer.h"

char *read_line(CircularBuffer *circular_buff, int *reachedEOF)
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

void	usage_msg( void )
{
    printf("USAGE: \n");
    printf("\tSINGLE\n\t<command>\n\n");
    printf("\tPIPED\n\t<command 1>\n\t<command 2>\n\n");
    printf("\tCONCURRENT\n\t<command>\n");
}

