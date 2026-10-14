#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "circularBuffer.h"
#include "splitCommand.h"

#define MAX_LINE 1024

int main() {
    // 1. Inicializar el buffer circular como pide la práctica
    CircularBuffer cb;
    if (buffer_init(&cb, MAX_LINE) != 0) {
        perror("Error al inicializar el buffer");
        return 1;
    }

    // Variables para almacenar las líneas leídas
    char modo[MAX_LINE];
    char comando1[MAX_LINE];
    char comando2[MAX_LINE];

    // Bucle infinito de la Shell
    while (1) {
        
        // --- PASO 1: LEER EL MODO DE EJECUCIÓN ---
        // NOTA: Para la entrega final usarás el buffer circular aquí.
        // De momento, usamos fgets para que puedas probar la lógica básica.
        if (fgets(modo, sizeof(modo), stdin) == NULL) {
            break; // Si llegamos al final del archivo (EOF), salimos del bucle
        }
        
        // Quitar el salto de línea '\n' que añade fgets al final
        modo[strcspn(modo, "\n")] = '\0';

        // --- PASO 2: EVALUAR EL MODO ---
        
        // Si el modo es EXIT, cerramos la shell de inmediato
        if (strcmp(modo, "EXIT") == 0) {
            break; 
        }

        // Si es un modo válido, leemos la siguiente línea que contiene el comando
        if (strcmp(modo, "SINGLE") == 0 || strcmp(modo, "CONCURRENT") == 0) {
            if (fgets(comando1, sizeof(comando1), stdin) == NULL) break;
            comando1[strcspn(comando1, "\n")] = '\0';
            
            // Trocear el comando en un arreglo de strings (ej: "ls", "-l", NULL)
            char **args = split_command(comando1);

            // Crear el proceso hijo
            pid_t pid = fork();

            if (pid < 0) {
                perror("Error en fork");
            } 
            else if (pid == 0) {
                // --- PROCESO HIJO ---
                // Reemplaza el programa actual con el comando del usuario
                if (execvp(args[0], args) == -1) {
                    perror("Error al ejecutar el comando");
                    exit(1);
                }
            } 
            else {
                // --- PROCESO PADRE (LA SHELL) ---
                if (strcmp(modo, "SINGLE") == 0) {
                    // Si es SINGLE, esperamos obligatoriamente a que el hijo termine
                    waitpid(pid, NULL, 0);
                } 
                else {
                    // Si es CONCURRENT, NO llamamos a waitpid aquí.
                    // Imprimimos un aviso y continuamos el bucle de inmediato.
                    printf("[Proceso corriendo en segundo plano con PID %d]\n", pid);
                }
            }
            
            // Liberar la memoria asignada por split_command
            free(args);
        }
        else if (strcmp(modo, "PIPED") == 0) {
            // --- MODO PIPED ---
            // 1. Leer comando 1
            if (fgets(comando1, sizeof(comando1), stdin) == NULL) break;
            comando1[strcspn(comando1, "\n")] = '\0';

            // 2. Leer comando 2
            if (fgets(comando2, sizeof(comando2), stdin) == NULL) break;
            comando2[strcspn(comando2, "\n")] = '\0';

            printf("Procesando tubería: %s -> %s\n", comando1, comando2);
            
            // Aquí irá tu lógica de pipe(), fork() x2, dup2() y waitpid() x2.
        }
    }

    // Liberar recursos antes de salir de la Shell
    buffer_deallocate(&cb);
    return 0;
}

