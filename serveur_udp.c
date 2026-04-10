#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 12345
#define BUFFER_SIZE 1024

int main() {
    int sock;
    struct sockaddr_in server, client;
    socklen_t client_len = sizeof(client);
    char buffer[BUFFER_SIZE];
    int compteur = 0;

    // 1. Création socket UDP
    sock = socket(AF_INET, SOCK_DGRAM, 0);

    // 2. Configuration serveur
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = INADDR_ANY;

    // 3. Bind
    bind(sock, (struct sockaddr*)&server, sizeof(server));

    printf("Serveur UDP en attente...\n");

    // 4. Réception
    while (1) {
        int n = recvfrom(sock, buffer, BUFFER_SIZE - 1, 0,
                         (struct sockaddr*)&client, &client_len);

        buffer[n] = '\0';
        compteur++;

        printf("Message %d : %s", compteur, buffer);
    }

    close(sock);
    return 0;
}