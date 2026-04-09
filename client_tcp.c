#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <time.h>

#define SERVER_IP "127.0.0.1"
#define PORT 12345
#define BUFFER_SIZE 1024

int main() {
    int sock_client;
    struct sockaddr_in addr_serveur;
    char message[BUFFER_SIZE];
    int compteur = 0;
    time_t t;
    struct tm *tm_info;

    /* 1) Création de la socket TCP */
    sock_client = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_client < 0) {
        perror("Erreur socket");
        return 1;
    }

    /* 2) Préparation de l'adresse du serveur */
    memset(&addr_serveur, 0, sizeof(addr_serveur));
    addr_serveur.sin_family = AF_INET;
    addr_serveur.sin_port = htons(PORT);

    if (inet_pton(AF_INET, SERVER_IP, &addr_serveur.sin_addr) <= 0) {
        perror("Erreur adresse IP");
        close(sock_client);
        return 1;
    }

    /* 3) Connexion au serveur */
    if (connect(sock_client, (struct sockaddr *)&addr_serveur, sizeof(addr_serveur)) < 0) {
        perror("Erreur connect");
        close(sock_client);
        return 1;
    }

    printf("Connecte au serveur %s:%d\n", SERVER_IP, PORT);

    /* 4) Envoi de Bonjour */
    strcpy(message, "Bonjour\n");
    send(sock_client, message, strlen(message), 0);
    compteur++;

    /* 5) Envoi 60 fois de l'heure courante */
    for (int i = 0; i < 60; i++) {
        t = time(NULL);
        tm_info = localtime(&t);

        strftime(message, BUFFER_SIZE, "Il est %H:%M:%S !\n", tm_info);

        send(sock_client, message, strlen(message), 0);
        compteur++;
    }

    /* 6) Envoi de Au revoir */
    strcpy(message, "Au revoir\n");
    send(sock_client, message, strlen(message), 0);
    compteur++;

    printf("Nombre total de messages envoyes : %d\n", compteur);

    /* 7) Fermeture */
    close(sock_client);

    return 0;
}