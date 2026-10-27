/*
 * Reads integers from standard input and prints the Fibonacci value for
 * each one. Each result is written as "Fib of n is value".
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

int fib(int n) {
    if (n <= 0)
        return 0;
    else if (n == 1)
    {
        return 1;
    }
    
    return fib(n-1) + fib(n-2);
}

void processNumber(char s[]) {
    char s2[100];
    int temp, temp2;
    sscanf(s, "%d", &temp);
    temp2 = fib(temp);
    sprintf(s2, "Fib of %d is %d\n", temp, temp2);
    write(1, s2, strlen(s2));
}

int main(int argc, char* argv[]) {
    int sum = 0;
    char s[80];
    int i = 0;
    while(read(0, &s[i], 1) > 0) {
        if (s[i] == '\n') {
            s[i] = 0;
            processNumber(s);
            i = 0;
        }
        else
            i++;
    }
    s[i] = '\0'; // why? comment this one
    processNumber(s);
}