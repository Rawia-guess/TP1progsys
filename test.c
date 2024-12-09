#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <time.h>  // Pour mesurer le temps d'exécution

// Messages et constantes
#define WELC_MES "Bienvenue dans le Shell ENSEA.\nPour quitter, tapez 'exit'.\n"
#define MAX_CMD_LENGTH 100
#define PROMPT "enseash % "

// Prototypes de fonctions
void disp_welc_mes();
void displayPrompt(int last_exit_status, bool is_signal, long exec_time);
void execute_command(char *command, int *last_exit_status, bool *is_signal, long *exec_time);
void handle_cd(char *path, int *last_exit_status);

void disp_welc_mes() {
    write(STDOUT_FILENO, WELC_MES, strlen(WELC_MES));
}
void displayPrompt(int last_exit_status, bool is_signal, long exec_time) {
    if (is_signal) {
        printf(" %s[sign:%d|%ldms] ", PROMPT, last_exit_status, exec_time);
    } else {
        printf(" %s[exit:%d|%ldms] ", PROMPT, last_exit_status, exec_time);
    }
}
int main() {
    disp_welc_mes();

    char command[MAX_CMD_LENGTH];
    int last_exit_status = 0;
    bool is_signal = false;
    long exec_time = 0;

    while (1) {
        displayPrompt(last_exit_status, is_signal, exec_time);

        // Lire la commande de l'utilisateur
        // Lire la commande de l'utilisateur
        if (fgets(command, sizeof(command), stdin) == NULL) {
            if (feof(stdin)) {
                write(STDOUT_FILENO, "\n", 1);
                break;
            } else {
                perror("Erreur de lecture de la commande");
                continue;
            }
        }
        command[strcspn(command, "\n")] = '\0';  // Enlever le caractère de nouvelle ligne

        if (strcmp(command, "exit") == 0) {
            write(STDOUT_FILENO, "Bye bye...\n", strlen("Bye bye...\n"));
            break;
        }

        execute_command(command, &last_exit_status, &is_signal, &exec_time);
    }

    return 0;
}
void handle_cd(char *path, int *last_exit_status) {
    if (chdir(path) != 0) {
        perror("cd");
        *last_exit_status = 1;
    }
}
void execute_command(char *command, int *last_exit_status, bool *is_signal, long *exec_time) {
    if (strncmp(command, "cd ", 3) == 0) {
        handle_cd(command + 3, last_exit_status);
        return;
    }

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    pid_t process_id = fork();
    if (process_id < 0) {
        perror("Fork failed");
        return;
    }
     if (process_id == 0) {  // Processus enfant
        char *arguments[MAX_CMD_LENGTH / 2 + 1];
        char *token = strtok(command, " ");
        int index = 0;

        while (token != NULL && index < MAX_CMD_LENGTH / 2) {
            arguments[index++] = token;
            token = strtok(NULL, " ");
        }
           arguments[index] = NULL;

        if (execvp(arguments[0], arguments) == -1) {
            perror("Commande non trouvée");
            exit(1);
        }
    } else {  // Processus parent
        int status;
        waitpid(process_id, &status, 0);

        clock_gettime(CLOCK_MONOTONIC, &end);
        *exec_time = (end.tv_sec - start.tv_sec) * 1000 + (end.tv_nsec - start.tv_nsec) / 1000000;

        if (WIFEXITED(status)) {
            *last_exit_status = WEXITSTATUS(status);
            *is_signal = false;
        } else if (WIFSIGNALED(status)) {
            *last_exit_status = WTERMSIG(status);
            *is_signal = true;
        }
    }
}
