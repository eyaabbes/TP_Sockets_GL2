#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <time.h>

#define PORT 12345
#define BUFFER_SIZE 1024

void reap_zombies(int sig) {
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

void traiter_client(int sock_client) {
    char buffer[BUFFER_SIZE];
    int nb_recu;
    int compteur = 0;

    printf("Processus %d : client pris en charge\n", getpid());

    while ((nb_recu = recv(sock_client, buffer, BUFFER_SIZE - 1, 0)) > 0) {
        buffer[nb_recu] = '\0';
        compteur++;

        printf("[PID %d] Message %d reçu : %s", getpid(), compteur, buffer);

        // délai volontaire demandé dans l'énoncé
        sleep(1);
    }

    if (nb_recu < 0) {
        perror("Erreur recv");
    }

    printf("[PID %d] Client terminé, total messages reçus : %d\n", getpid(), compteur);
    close(sock_client);
    exit(0);
}

int main() {
    int sock_serveur, sock_client;
    struct sockaddr_in addr_serveur, addr_client;
    socklen_t taille_client;

    signal(SIGCHLD, reap_zombies);

    sock_serveur = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_serveur < 0) {
        perror("Erreur socket");
        return 1;
    }

    int opt = 1;
    setsockopt(sock_serveur, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    memset(&addr_serveur, 0, sizeof(addr_serveur));
    addr_serveur.sin_family = AF_INET;
    addr_serveur.sin_port = htons(PORT);
    addr_serveur.sin_addr.s_addr = INADDR_ANY;

    if (bind(sock_serveur, (struct sockaddr *)&addr_serveur, sizeof(addr_serveur)) < 0) {
        perror("Erreur bind");
        close(sock_serveur);
        return 1;
    }

    if (listen(sock_serveur, 10) < 0) {
        perror("Erreur listen");
        close(sock_serveur);
        return 1;
    }

    printf("Serveur concurrent en attente sur le port %d...\n", PORT);

    while (1) {
        taille_client = sizeof(addr_client);
        sock_client = accept(sock_serveur, (struct sockaddr *)&addr_client, &taille_client);

        if (sock_client < 0) {
            perror("Erreur accept");
            continue;
        }

        pid_t pid = fork();

        if (pid < 0) {
            perror("Erreur fork");
            close(sock_client);
            continue;
        }

        if (pid == 0) {
            // fils
            close(sock_serveur);
            traiter_client(sock_client);
        } else {
            // père
            close(sock_client);
        }
    }

    close(sock_serveur);
    return 0;
}