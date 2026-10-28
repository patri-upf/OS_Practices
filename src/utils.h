#pragma once

#define MAX_COMMAND 60
#define MAX_ARGS    512
#define MAX_SIZE    2052

#include "circularBuffer.h"

char	*read_line(CircularBuffer *circular_buff, int *reachedEOF);
void	usage_msg( void );
void    free_args(char **args);
