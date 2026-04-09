#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>

#define PORT 80
#define BUFFER_SIZE 4096
#define REQUEST_SIZE 8192

int main() {
    int sock;
    struct sockaddr_in server;
    char hostname[100];
    char request[REQUEST_SIZE];
    char buffer[BUFFER_SIZE];
    int bytes;

    // 1. Demander le serveur
    printf("Entrez le serveur (ex: google.com) : ");
    scanf("%s", hostname);

    // 2. Résolution DNS
    struct hostent *he = gethostbyname(hostname);
    if (he == NULL) {
        printf("Erreur: serveur introuvable\n");
        return 1;
    }

    // 3. Création socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        return 1;
    }

    // 4. Configuration serveur
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    memcpy(&server.sin_addr, he->h_addr_list[0], he->h_length);

    // 5. Connexion
    if (connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
        perror("connect");
        return 1;
    }

    printf("\nConnecté au serveur %s\n", hostname);

    // vider le buffer après scanf
    getchar();

    // 6. Lecture requête HTTP
    printf("\nTapez votre requête HTTP :\n");
    printf("(Terminez par une ligne vide)\n\n");

    request[0] = '\0';

    while (1) {
        char line[1024];

        if (fgets(line, sizeof(line), stdin) == NULL)
            break;

        if (strcmp(line, "\n") == 0) {
            strncat(request, "\r\n", REQUEST_SIZE - strlen(request) - 1);
            break;
        }

        strncat(request, line, REQUEST_SIZE - strlen(request) - 1);
    }

    // 7. Envoi
    send(sock, request, strlen(request), 0);

    printf("\n----- Requête envoyée -----\n%s\n", request);

    // 8. Réception
    printf("\n----- Réponse serveur -----\n");

    while ((bytes = recv(sock, buffer, BUFFER_SIZE - 1, 0)) > 0) {
        buffer[bytes] = '\0';
        printf("%s", buffer);
    }

    // 9. Fermeture
    close(sock);

    printf("\n\n----- Fin -----\n");

    return 0;
}