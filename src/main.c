#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "circularBuffer.h"
#include "executor.h"
#include "utils.h"

volatile sig_atomic_t last_concurrent_pid = -1;

void	handle_sigint(int sig)
{
	if (last_concurrent_pid > 0)
		kill(last_concurrent_pid, SIGKILL);
}

int main( void )
{
    CircularBuffer circular_buff;
    char    *line;
    int     reachedEOF = 0;

    if (buffer_init(&circular_buff, MAX_SIZE))
        return EXIT_FAILURE;

    signal(SIGINT, handle_sigint);

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
