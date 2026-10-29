#pragma once

#include <sys/wait.h>
#include <string.h>

#include "circularBuffer.h"
#include "splitCommand.h"
#include "utils.h"

int	executor(char **args);
int	execute_single(CircularBuffer *circular_buff, int *reachedEOF);
int	execute_piped(CircularBuffer *circular_buff, int *reachedEOF);
int	execute_concurrent(CircularBuffer *circular_buff, int *reachedEOF);
int	execute_timeout(CircularBuffer *circular_buff, int *reachedEOF);

