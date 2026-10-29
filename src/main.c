#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#include "circularBuffer.h"
#include "executor.h"
#include "utils.h"

int main( void )
{
    CircularBuffer circular_buff;
    char    *line;
    int     reachedEOF = 0;

    if (buffer_init(&circular_buff, MAX_SIZE))
        return EXIT_FAILURE;

    while ((line = read_line(&circular_buff, &reachedEOF)) != NULL)
    {
		if (strcmp(line, "TIMEOUT") == 0)
		{
			if(execute_timeout(&circular_buff, &reachedEOF)) 
			{
				free(line);
				exit(EXIT_FAILURE);
			}
		}
        else if (strcmp(line, "SINGLE") == 0)
        {
	    	if (execute_single(&circular_buff, &reachedEOF))
	    	{
				free(line);
				exit(EXIT_FAILURE);
	    	}
        }
        else if (strcmp(line, "PIPED") == 0)
		{
	    	if (execute_piped(&circular_buff, &reachedEOF))
			{
				free(line);
				exit(EXIT_FAILURE);
			}
		}
		else if(strcmp(line, "CONCURRENT") == 0)
		{
			if (execute_concurrent(&circular_buff, &reachedEOF))
			{
            	free(line);
				exit(EXIT_FAILURE);
        	}
		}
        else if(strcmp(line, "EXIT") == 0)
        {
            printf("BYE :)\n\tSee you soon...\n");
            free(line);
            break ;
        }
        else
	    	usage_msg();

	free(line);
    }
}
