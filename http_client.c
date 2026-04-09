#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_IP "142.250.74.14"   // IP Google (peut changer mais fonctionne)
#define SERVER_PORT 80
#define BUFFER_SIZE 4096

int main() {
    int sock;
    struct sockaddr_in server;
    char request[] = "GET / HTTP/1.1\r\nHost: google.com\r\nConnection: close\r\n\r\n";
    char buffer[BUFFER_SIZE];
    int bytes;

    // 1. Création socket TCP
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        return 1;
    }

    // 2. Configuration serveur
    server.sin_family = AF_INET;
    server.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP, &server.sin_addr);

    // 3. Connexion
    if (connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
        perror("connect");
        return 1;
    }

    printf("Connecté au serveur\n");

    // 4. Envoi requête HTTP
    send(sock, request, strlen(request), 0);

    printf("Requête envoyée :\n%s\n", request);

    // 5. Réception réponse
    while ((bytes = recv(sock, buffer, BUFFER_SIZE - 1, 0)) > 0) {
        buffer[bytes] = '\0';
        printf("%s", buffer);
    }

    // 6. Fermeture
    close(sock);

    return 0;
}