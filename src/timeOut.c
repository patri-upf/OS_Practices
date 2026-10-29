#include "timeOut.h"

// Look if the line contains the timeout correctly
char    *timeout_command(CircularBuffer *circular_buff, int *reachedEOF)
{
    char    *line = read_line(circular_buff, reachedEOF);

    if (!line)
        return NULL;

    if (strncmp(line, "TIMEOUT", 7) == 0)
    {
        char    *space = strchr(line, ' ');

        if (space != NULL)
        {
            timeout_time = atoi(space+1);
            free(line);
            return read_line(circular_buff, reachedEOF);
        }
    }

    timeout_time = -1;
    return line;
}

// ativates the watch and kill the process that is not done
void    timeout_wait(pid_t exec_pid)
{
    int seconds;
    pid_t   watcher_pid, finished_first;

    if (timeout_time <= 0)
    {
        waitpid(exec_pid, NULL, 0);
        return ;
    }

    seconds = timeout_time;
    timeout_time = -1;

    watcher_pid = fork();
    if (watcher_pid == 0)
    {
        sleep(seconds);
        printf("ERROR: timeout\n");
        exit(EXIT_SUCCESS);
    }

    finished_first = wait(NULL);
    if (finished_first == exec_pid)
        kill(watcher_pid, SIGKILL);
    else
        kill(exec_pid, SIGKILL);

    waitpid(exec_pid, NULL, 0);
    waitpid(watcher_pid, NULL, 0);
}
