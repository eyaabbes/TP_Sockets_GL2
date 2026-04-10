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
    if (sock < 0) {
        perror("Erreur socket");
        return 1;
    }

    // 2. Configuration serveur
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = INADDR_ANY;

    // 3. Bind
    if (bind(sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
        perror("Erreur bind");
        return 1;
    }

    printf("Serveur UDP avec delay en attente...\n");

    // 4. Réception
    while (1) {
        int n = recvfrom(sock, buffer, BUFFER_SIZE - 1, 0,
                         (struct sockaddr*)&client, &client_len);

        if (n < 0) {
            perror("Erreur recvfrom");
            continue;
        }

        buffer[n] = '\0';
        compteur++;

        printf("Message %d reçu : %s", compteur, buffer);

        
        sleep(1);
    }

    close(sock);
    return 0;
}