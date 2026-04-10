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
   char buffer[1024];
int n = recv(sock_client, buffer, 1023, 0);
buffer[n] = '\0';

printf("Requête reçue : %s\n", buffer);

// 🔴 SERVICE 1 : TIME
if (strncmp(buffer, "TIME", 4) == 0) {
    sleep(5);  // pour voir le parallélisme

    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);

    char rep[1024];
    strftime(rep, 1024, "Heure : %H:%M:%S\n", tm_info);
    send(sock_client, rep, strlen(rep), 0);
}

// 🔴 SERVICE 2 : NPROC
else if (strncmp(buffer, "NPROC", 5) == 0) {
    sleep(5);

    FILE *fp = popen("ps -e | wc -l", "r");
    char rep[1024];

    fgets(rep, 1024, fp);
    send(sock_client, rep, strlen(rep), 0);
    pclose(fp);
}

// 🔴 SERVICE 3 : ECHO
else if (strncmp(buffer, "ECHO ", 5) == 0) {
    sleep(5);

    send(sock_client, buffer + 5, strlen(buffer + 5), 0);
}

else {
    char *msg = "Service inconnu\n";
    send(sock_client, msg, strlen(msg), 0);
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