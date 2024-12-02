#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define PROMPT enseash%
#define MAX_CMD_LENGTH 1024

int main() {
    char command[MAX_CMD_LENGTH];

    // Affichage du message d'accueil
    printf("Bienvenue dans le Shell ENSEA.\n");

    while (1) {
        // Affichage du prompt
        printf("$ ");
        
        // Lecture de la commande
        if (fgets(command, sizeof(command), stdin) == NULL) {
            perror("fgets failed");
            continue;
        }// Supprimer le caractère de nouvelle ligne
        command[strcspn(command, "\n")] = 0;

        // Quitter si l'utilisateur entre 'quit'
        if (strcmp(command, "quit") == 0) {
            printf("Quitting the shell. Goodbye!\n");
            break;
        }

        // Exécution de la commande
        int result = system(command);
        if (result == -1) {
            perror("Erreur lors de l'exécution de la commande");
        }
    }

    return 0;
}