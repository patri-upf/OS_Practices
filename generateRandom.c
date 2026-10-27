/*
 * Generates nSamples random integers in the range [0, nMax) and writes
 * one integer per line to standard output.
 * Usage: generateRandom nSamples nMax
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

int main(int argc, char* argv[]) {
    char s[120];
    if (argc != 3){
        sprintf(s, "Usage generateRandom nSamples nMax\n");
        write(1, s, strlen(s));
        _exit(1);
    }
    int nSamples, nMax;
    sscanf(argv[1], "%d", &nSamples);
    sscanf(argv[2], "%d", &nMax);
    for (int i =0; i <nSamples; i ++) {
        int r = random()%nMax;
        sprintf(s, "%d\n", r);
        write(1, s, strlen(s));
    }
}