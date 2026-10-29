#include "executor.h"

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

    return 0;
}

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
				if (executor(args) < 0)	
				{
		    		perror("Error in execvp SINGLE\n");
		    		free(command);
		    		return EXIT_FAILURE;
				}
	    	}
	    	else if (pid_1 > 0)
				waitpid(pid_1, NULL, 0);

	    	free(command);
			free(args);
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

			if (executor(args_1) < 0)
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
				if (executor(args_2) < 0)
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
				if (executor(args) < 0)
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

		pid_t executor_pid = fork();
		if (executor_pid == 0)
		{
			executor(args);
			perror("execvp TIMEOUT");
			exit(EXIT_FAILURE);
		}

		pid_t watcher_pid = fork();
		if (watcher_pid == 0)
		{
			sleep(seconds);
			printf("\tERROR: timeout\n");
			exit(EXIT_SUCCESS);
		}

		pid_t end_child = wait(NULL);          
		if (end_child == executor_pid)
			kill(watcher_pid, SIGKILL);        
		else
			kill(executor_pid, SIGKILL);       

		waitpid(executor_pid, NULL, 0);
		waitpid(watcher_pid, NULL, 0);

		free(args);
	}

	free(str_time);
	free(command);

	return (EXIT_SUCCESS);
}


