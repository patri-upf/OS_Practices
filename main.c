#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#include "circularBuffer.h"
#include "splitCommand.h"
#include "utils.h"

int	end = 0;

int	execute_single(CircularBuffer *circular_buff, int *reachedEOF)
{
    char	*command;
    char	**args;
    pid_t	pid_1;

    command = read_line(circular_buff, reachedEOF);

    if (command!= NULL)
    {
		args = split_command(command);
		if (args != NULL)
		{
	    	pid_1 = fork();
	    	if (pid_1 == 0)
	    	{
				if (execvp(args[0], args) < 0)
				{
		    		perror("Error in execvp SINGLE\n");
		    		free(command);
		    		return EXIT_FAILURE;
				}
	    	}
	    	else if (pid_1 > 0)
				waitpid(pid_1, NULL, 0);

	    		free(command);
		}
    }
    
	return EXIT_SUCCESS;
}

int	execute_piped(CircularBuffer *circular_buff, int *reachedEOF)
{
    char	*command_1;
    char	*command_2;
    char	**args_1;
    char	**args_2;
    pid_t	pid_1, pid_2;
    int		fd[2];

    command_1 = read_line(circular_buff, reachedEOF);

    if (command_1 != NULL)
    {
		args_1 = split_command(command_1);

		if (args_1 != NULL)
		{
			if (pipe(fd) < 0)
			perror("ERORR");
		}

		pid_1 = fork();

		if (pid_1 < 0)
			perror("fork");

		if (pid_1 == 0)
		{
			dup2(fd[1], STDOUT_FILENO);
				close(fd[1]);
				close(fd[0]);

			if (execvp(args_1[0], args_1) < 0)
			{
			perror("Error in execvp PIPED\n");
			return EXIT_FAILURE;
			}
		}
    }

    free(command_1);
    free(args_1);

    command_2 = read_line(circular_buff, reachedEOF);

    if (command_2 != NULL)
    {
	args_2 = split_command(command_2);
	if (args_2 != NULL)
	{
	    pid_2 = fork();
	    if (pid_2 < 0)
		perror("fork");

	    if (pid_2 == 0)
	    {
			dup2(fd[0], STDIN_FILENO);
			close(fd[0]);
			close(fd[1]);
			if (execvp(args_2[0], args_2) < 0)
			{
				perror("Error in execvp PIPED\n");
				return EXIT_FAILURE;
			}
	    }
	}

	close(fd[1]);
	close(fd[0]);
	waitpid(pid_1, NULL, 0);
	waitpid(pid_2, NULL, 0);
	free(command_2);
	free(args_2);
    }

    return EXIT_SUCCESS;
}

int	execute_concurrent(CircularBuffer *circular_buff, int *reachedEOF)
{
    char	*command;
    char	**args;
    pid_t	pid;

    command = read_line(circular_buff, reachedEOF);

    if (command != NULL)
    {
	args = split_command(command);

	if (args != NULL)
	{
	    pid = fork();

	    if (pid == 0)
	    {
		if (execvp(args[0], args) < 0)
		{
		    perror("Error en execvp CONCURRENT\n");
		    return EXIT_FAILURE;
		}
	    }

	    free(args);
	}

	free(command);
    }

    return EXIT_SUCCESS;
}

int execute_timeout(CircularBuffer *circular_buff, int *reachedEOF)
{
	char	*str_time = read_line(circular_buff, reachedEOF);
	char	*command = read_line(circular_buff, reachedEOF);

	if (str_time && command)
	{
		int		seconds = atoi(str_time);
		char	**args = split_command(command);

		pid_t executor = fork();
		if (executor == 0)
		{
			execvp(args[0], args);
			perror("execvp TIMEOUT");
			exit(EXIT_FAILURE);
		}

		pid_t watcher = fork();
		if (watcher == 0)
		{
			sleep(seconds);
			printf("\tERROR: timeout\n");
			exit(EXIT_SUCCESS);
		}

		pid_t end_child = wait(NULL);          
		if (end_child == executor)
			kill(watcher, SIGKILL);        
		else
			kill(executor, SIGKILL);       

		waitpid(executor, NULL, 0);
		waitpid(watcher, NULL, 0);

		free(args);
	}
	free(str_time);
	free(command);

	return (EXIT_SUCCESS);
}


int main( void )
{
    CircularBuffer circular_buff;
    char    *line;
    int     reachedEOF = 0;

    if (buffer_init(&circular_buff, MAX_SIZE))
        return EXIT_FAILURE;

    while (((line = read_line(&circular_buff, &reachedEOF)) != NULL) && !end)
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
				exit(EXIT_FAILURE);
		}
		else if(strcmp(line, "CONCURRENT") == 0)
		{
			if (execute_concurrent(&circular_buff, &reachedEOF))
				exit(EXIT_FAILURE);
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
