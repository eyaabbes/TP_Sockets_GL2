#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <time.h>

#define PORT 12345
#define BUFFER_SIZE 1024

void traiter_client(int sock_client) {
    char buffer[BUFFER_SIZE];
    int n = recv(sock_client, buffer, BUFFER_SIZE - 1, 0);

    if (n <= 0) {
        close(sock_client);
        exit(0);
    }

    buffer[n] = '\0';
    printf("Processus %d traite la requête : %s\n", getpid(), buffer);
    printf("Requête reçue : %s\n", buffer);

    // 🔴 SERVICE TIME
    if (strncmp(buffer, "TIME", 4) == 0) {
        sleep(5);  // pour voir le parallélisme

        char rep[BUFFER_SIZE];
        time_t t = time(NULL);
        struct tm *tm_info = localtime(&t);

        strftime(rep, BUFFER_SIZE, "Heure : %H:%M:%S\n", tm_info);
        send(sock_client, rep, strlen(rep), 0);
    }

    // 🔴 SERVICE NPROC
    else if (strncmp(buffer, "NPROC", 5) == 0) {
        sleep(5);

        FILE *fp = popen("ps -e | wc -l", "r");
        char rep[BUFFER_SIZE];

        fgets(rep, BUFFER_SIZE, fp);
        send(sock_client, rep, strlen(rep), 0);

        pclose(fp);
    }

    // 🔴 SERVICE ECHO
    else if (strncmp(buffer, "ECHO ", 5) == 0) {
        sleep(5);

        send(sock_client, buffer + 5, strlen(buffer + 5), 0);
    }

    else {
        char *msg = "Service inconnu\n";
        send(sock_client, msg, strlen(msg), 0);
    }

    close(sock_client);
    exit(0);
}

int main() {
    int sock_serveur, sock_client;
    struct sockaddr_in addr_serveur, addr_client;
    socklen_t taille_client;

    // création socket
    sock_serveur = socket(AF_INET, SOCK_STREAM, 0);

    addr_serveur.sin_family = AF_INET;
    addr_serveur.sin_port = htons(PORT);
    addr_serveur.sin_addr.s_addr = INADDR_ANY;

    bind(sock_serveur, (struct sockaddr*)&addr_serveur, sizeof(addr_serveur));
    listen(sock_serveur, 5);

    printf("Serveur multi-services en attente...\n");

    while (1) {
        taille_client = sizeof(addr_client);
        sock_client = accept(sock_serveur, (struct sockaddr*)&addr_client, &taille_client);

        if (sock_client < 0) {
            perror("accept");
            continue;
        }

        pid_t pid = fork();

        if (pid == 0) {
            // 🔴 FILS
            close(sock_serveur);
            traiter_client(sock_client);
        } else {
            // 🔴 PERE
            close(sock_client);
        }
    }

    close(sock_serveur);
    return 0;
}