#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "circularBuffer.h"
#include "splitCommand.h"

#define MAX_COMMAND 60
#define MAX_ARGS    512
#define MAX_SIZE    2052

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

int main( void )
{
    CircularBuffer circular_buff;
    char    *line;
    char    *command_1;
    char    *command_2;
    char    **args_1;
    char    **args_2;
    int     reachedEOF = 0;
    pid_t   pid_1, pid_2;
    int     fd[2];
    

    if (buffer_init(&circular_buff, MAX_SIZE))
        return EXIT_FAILURE;

    while ((line = read_line(&circular_buff, &reachedEOF)) != NULL)
    {
        if (strcmp(line, "SINGLE") == 0)
        {
            // lee la linea que tiene el comando
            command_1 = read_line(&circular_buff, &reachedEOF);
            
            if (command_1!= NULL)
            {
                args_1 = split_command(command_1);

                if (args_1 != NULL)
                {
                    pid_1 = fork();
                    if (pid_1 == 0)
                    {
                    // el proceso hijo ejecuta el comando
                        if (execvp(args_1[0], args_1) < 0)
                        {
                            perror("Error en execvp SINGLE");
                            free(command_1);
                            free(line);
                            exit(EXIT_FAILURE);
                        }
                    }
                    else if (pid_1 > 0)
                        // mientras el padre espera al hijo
                        waitpid(pid_1, NULL, 0);

                    free(command_1);
                }
            }
        }
        else if (strcmp(line, "PIPED") == 0) {
            command_1 = read_line(&circular_buff, &reachedEOF);
            if (command_1 != NULL) {
                args_1 = split_command(command_1);
                if (args_1 != NULL) {
                    if (pipe(fd) < 0) {
                        perror("ERORR");
                    }

                    pid_1 = fork();
                    if (pid_1 < 0) {
                        perror("fork");
                    }
                    if (pid_1 == 0) {           //Is the child
                        dup2(fd[1], STDOUT_FILENO);
                        close(fd[1]);
                        close(fd[0]);
                        if (execvp(args_1[0], args_1) < 0) {
                            perror("Error in execvp PIPED");
                            exit(EXIT_FAILURE);
                        }

                    }
                }
                free(command_1);
                free(args_1);
            }
            command_2 = read_line(&circular_buff, &reachedEOF);
            if (command_2 != NULL) {
                args_2 = split_command(command_2);
                if (args_2 != NULL) {
                    pid_2 = fork();
                    if (pid_2 < 0) {
                        perror("fork");
                    }
                    if (pid_2 == 0) {
                        dup2(fd[0], STDIN_FILENO);
                        close(fd[0]);
                        close(fd[1]);
                        if (execvp(args_2[0], args_2) < 0) {
                            perror("Error in execvp PIPED");
                            _exit(EXIT_FAILURE);
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
            free(line);
        }
        else if(strcmp(line, "CONCURRENT") == 0)
        {
            // el padre continua y el hijo espera
            // execute_concurrent();
            command_1 = read_line(&circular_buff, &reachedEOF);
            
            if (command_1 != NULL)
            {
                args_1 = split_command(command_1);

                if (args_1 != NULL)
                {
                    pid_1 = fork();
                    if (pid_1 == 0)
                    {
                        if (execvp(args_1[0], args_1) < 0)
                        {
                            perror("Error en execvp CONCURRENT");
                            exit(EXIT_FAILURE);
                        }
                    }

                    // no espera al padre (proceso zombie...)
                    free(args_1);
                }

                free(command_1);
            }
        }
        else if(strcmp(line, "EXIT") == 0)
        {
            printf("BYE :)\n\tSee you soon...\n");
            free(line);
            break ;
        }
        else
        {
            printf("USAGE: \n");
        }
    }
}
