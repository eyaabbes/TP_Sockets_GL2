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

    sock_client = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_client < 0) {
        perror("Erreur socket");
        return 1;
    }

    memset(&addr_serveur, 0, sizeof(addr_serveur));
    addr_serveur.sin_family = AF_INET;
    addr_serveur.sin_port = htons(PORT);

    if (inet_pton(AF_INET, SERVER_IP, &addr_serveur.sin_addr) <= 0) {
        perror("Erreur adresse IP");
        close(sock_client);
        return 1;
    }

    if (connect(sock_client, (struct sockaddr *)&addr_serveur, sizeof(addr_serveur)) < 0) {
        perror("Erreur connect");
        close(sock_client);
        return 1;
    }

    printf("Client %d connecté\n", getpid());

    for (int i = 1; i <= 5; i++) {
        snprintf(message, BUFFER_SIZE, "Client PID=%d, message %d\n", getpid(), i);
        send(sock_client, message, strlen(message), 0);
    }

    close(sock_client);
    return 0;
}