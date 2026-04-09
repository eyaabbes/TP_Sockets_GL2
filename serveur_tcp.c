#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 12345
#define BUFFER_SIZE 1024

int main() {
    int sock_serveur, sock_client;
    struct sockaddr_in addr_serveur, addr_client;
    socklen_t taille_client;
    char buffer[BUFFER_SIZE];
    int nb_recu;
    int compteur = 0;

    /* 1) Création de la socket TCP du serveur */
    sock_serveur = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_serveur < 0) {
        perror("Erreur socket");
        return 1;
    }

    /* 2) Préparation de l'adresse du serveur */
    memset(&addr_serveur, 0, sizeof(addr_serveur));
    addr_serveur.sin_family = AF_INET;
    addr_serveur.sin_port = htons(PORT);
    addr_serveur.sin_addr.s_addr = INADDR_ANY;

    /* 3) Association de la socket à l'adresse et au port */
    if (bind(sock_serveur, (struct sockaddr *)&addr_serveur, sizeof(addr_serveur)) < 0) {
        perror("Erreur bind");
        close(sock_serveur);
        return 1;
    }

    /* 4) Mise en écoute */
    if (listen(sock_serveur, 5) < 0) {
        perror("Erreur listen");
        close(sock_serveur);
        return 1;
    }

    printf("Serveur en attente sur le port %d...\n", PORT);

    /* 5) Acceptation d'un client */
    taille_client = sizeof(addr_client);
    sock_client = accept(sock_serveur, (struct sockaddr *)&addr_client, &taille_client);
    if (sock_client < 0) {
        perror("Erreur accept");
        close(sock_serveur);
        return 1;
    }

    printf("Client connecte !\n");

    /* 6) Réception des messages */
    while ((nb_recu = recv(sock_client, buffer, BUFFER_SIZE - 1, 0)) > 0) {
        buffer[nb_recu] = '\0';
        compteur++;
        printf("Message %d recu : %s", compteur, buffer);
    }

    if (nb_recu < 0) {
        perror("Erreur recv");
    }

    printf("\nNombre total de messages recus : %d\n", compteur);

    /* 7) Fermeture */
    close(sock_client);
    close(sock_serveur);

    return 0;
}