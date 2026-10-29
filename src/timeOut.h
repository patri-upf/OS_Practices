#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>

#include "circularBuffer.h"
#include "utils.h"

extern volatile sig_atomic_t timeout_time;

char    *timeout_command(CircularBuffer *circular_buff, int *reachedEOF);
void    timeout_wait(pid_t executor_pid);
