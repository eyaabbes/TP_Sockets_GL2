#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>

#define SERVER_IP "127.0.0.1"
#define PORT 12345
#define BUFFER_SIZE 1024

int main() {
    int sock;
    struct sockaddr_in server;
    char message[BUFFER_SIZE];
    int compteur = 0;

    // 1. Socket UDP
    sock = socket(AF_INET, SOCK_DGRAM, 0);

    // 2. Config serveur
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    inet_pton(AF_INET, SERVER_IP, &server.sin_addr);

    // 3. Envoi Bonjour
    strcpy(message, "Bonjour\n");
    sendto(sock, message, strlen(message), 0,
           (struct sockaddr*)&server, sizeof(server));
    compteur++;

    // 4. Envoi 60 fois l'heure
    for (int i = 0; i < 60; i++) {
        time_t t = time(NULL);
        struct tm *tm_info = localtime(&t);

        strftime(message, BUFFER_SIZE, "Il est %H:%M:%S\n", tm_info);

        sendto(sock, message, strlen(message), 0,
               (struct sockaddr*)&server, sizeof(server));
        compteur++;
    }

    // 5. Au revoir
    strcpy(message, "Au revoir\n");
    sendto(sock, message, strlen(message), 0,
           (struct sockaddr*)&server, sizeof(server));
    compteur++;

    printf("Messages envoyés : %d\n", compteur);

    close(sock);
    return 0;
}