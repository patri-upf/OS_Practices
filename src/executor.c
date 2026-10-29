#include "executor.h"

extern volatile sig_atomic_t last_concurrent_pid;
extern volatile sig_atomic_t timeout_time;

int	executor(char **args)
{
    if (!args || !args[0])
		return -1;

    char path[256];
    char *exec_cmd = args[0];

    if (strcmp(args[0], "fibonacci") == 0 || strcmp(args[0], "generateRandom") == 0)
    {
        snprintf(path, sizeof(path), "./%s", args[0]);
        exec_cmd = path;
    }

    if (execvp(exec_cmd, args) < 0)
		return -1;

    return EXIT_SUCCESS;
}

int execute_single(CircularBuffer *circular_buff, int *reachedEOF)
{
	pid_t	pid;
	char	**args;
    char *command = timeout_command(circular_buff, reachedEOF);

    if (command != NULL)
    {
        args = split_command(command);
        if (args != NULL)
        {
            pid = fork();
            if (pid == 0)
            {
                if (executor(args) < 0)
                {
                    perror("execvp SINGLE");
                    exit(EXIT_FAILURE);
                }
            }
            else if (pid > 0)
                timeout_wait(pid);

			free(args);
        }

		free(command);
    }

    return EXIT_SUCCESS;
}
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include "circularBuffer.h"

extern volatile sig_atomic_t timeout_time;

int	execute_piped(CircularBuffer *circular_buff, int *reachedEOF)
{
    char	*command_1;
    char	*command_2;
    char	**args_1;
    char	**args_2;
    pid_t	pid_1, pid_2;
    int		fd[2];

    command_1 = timeout_command(circular_buff, reachedEOF);

    if (command_1 != NULL)
    {
		if (pipe(fd) < 0)
			perror("ERROR pipe");
			
		args_1 = split_command(command_1);

		if (args_1 != NULL)
		{
			if (pipe(fd) < 0) { perror("ERORR pipe"); }
			
			pid_1 = fork();
			if (pid_1 < 0) { perror("ERROR pid_1"); }

			if (pid_1 == 0)
			{
				dup2(fd[1], STDOUT_FILENO);
				close(fd[1]);
				close(fd[0]);

				if (executor(args_1) < 0)
				{
					perror("Error in execvp PIPED\n");
					free(args_1); free(command_1);
					exit(EXIT_FAILURE);
				}
			}
		}

    	free(command_1);
    	free(args_1);
	}

    command_2 = read_line(circular_buff, reachedEOF);

    if (command_2 != NULL)
    {
		args_2 = split_command(command_2);
		if (args_2 != NULL)
		{
		    pid_2 = fork();
		    if (pid_2 < 0) { perror("fork"); }

		    if (pid_2 == 0)
		    {
				dup2(fd[0], STDIN_FILENO);
				close(fd[0]);
				close(fd[1]);
				
				if (executor(args_2) < 0)
				{
					perror("Error in execvp PIPED\n");
					free(args_2); free(command_2);
					exit(EXIT_FAILURE);
				}
		    }
		}

		close(fd[1]);
		close(fd[0]);

		if (timeout_time > 0)
		{
			int	seconds = timeout_time;
			timeout_time = -1;

			pid_t	watcher_pid = fork();
			if (watcher_pid == 0)
			{
				sleep(seconds);
				printf("ERROR timeout");

				if (pid_1 > 0) { kill(pid_1, SIGKILL); }
				if (pid_2 > 0) { kill(pid_2, SIGKILL); }

				exit(EXIT_SUCCESS);
			}

			pid_t	finished_first = wait(NULL);
			if (finished_first == watcher_pid)
			{
				if (pid_1 > 0) { kill(pid_1, SIGKILL); }
				if (pid_2 > 0) { kill(pid_2, SIGKILL); }
			} else { 
				kill(watcher_pid, SIGKILL); 
			}

			waitpid(watcher_pid, NULL, 0);
			if (pid_1 > 0) { waitpid(pid_1, NULL, 0); }
			if (pid_2 > 0) { waitpid(pid_2, NULL, 0); }
		}
		else
		{
			if (pid_1 > 0) { waitpid(pid_1, NULL, 0); }
			if (pid_2 > 0) { waitpid(pid_2, NULL, 0); }
		}

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
				signal(SIGINT, SIG_DFL);
				if (executor(args) < 0)
				{
					perror("Error en execvp CONCURRENT\n");
					return EXIT_FAILURE;
				}
			}
			else if (pid > 0)
			{
				last_concurrent_pid = pid;

				if (timeout_time > 0)
				{
					int	seconds = timeout_time;
					timeout_time = -1;

					if (fork() == 0)
					{
						sleep(seconds);
						printf("ERROR: timeout\n");
						kill(pid, SIGKILL);
						exit(EXIT_SUCCESS);
					}
				}
			}

			free(args);
		}

		free(command);
    }

    return EXIT_SUCCESS;
}

int	execute_timeout(CircularBuffer *circular_buff, int *reachedEOF)
{
	char	*str_time = read_line(circular_buff, reachedEOF);
	char	*command = read_line(circular_buff, reachedEOF);

	if (str_time != NULL && command != NULL)
	{
		timeout_time = atoi(str_time);
		char	**args = split_command(command);

		if (args != NULL)
		{
			pid_t exec_pid= fork();
			if (exec_pid == 0)
			{
				if (executor(args) < 0)
				{
					perror("ERROR in exec timeout\n");
					free(args);
					free(command);
					free(str_time);
					exit(EXIT_FAILURE);
				}
			}
			else if (exec_pid > 0) {
				timeout_wait(exec_pid);
			}

			free(args);
		}
	}

	free(str_time);
	free(command);

	return (EXIT_SUCCESS);
}


