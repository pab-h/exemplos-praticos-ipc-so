#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>       // close
#include <arpa/inet.h>    // inet_pton
#include <pthread.h>

#define HOST "127.0.0.1"
#define PORT 9000
#define BUFFER_SIZE 1024

int sock;

// Thread para ouvir mensagens do servidor
void* ouvir(void* arg) {
    char buffer[BUFFER_SIZE];

    while (1) {
        int bytes = recv(sock, buffer, BUFFER_SIZE - 1, 0);
        if (bytes <= 0) {
            printf("Conexão encerrada.\n");
            break;
        }
        buffer[bytes] = '\0';
        printf("%s\n", buffer);
    }

    return NULL;
}

int main() {
    struct sockaddr_in server_addr;
    pthread_t thread;

    // Criar socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Erro ao criar socket");
        return 1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    // Converter IP
    if (inet_pton(AF_INET, HOST, &server_addr.sin_addr) <= 0) {
        perror("Endereço inválido");
        return 1;
    }

    // Conectar ao servidor
    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Erro na conexão");
        return 1;
    }

    char nome[BUFFER_SIZE];
    printf("Digite seu nome: ");
    fgets(nome, BUFFER_SIZE, stdin);
    nome[strcspn(nome, "\n")] = '\0'; // remover \n

    send(sock, nome, strlen(nome), 0);

    // Criar thread para ouvir mensagens
    if (pthread_create(&thread, NULL, ouvir, NULL) != 0) {
        perror("Erro ao criar thread");
        return 1;
    }

    char msg[BUFFER_SIZE];

    while (1) {
        if (fgets(msg, BUFFER_SIZE, stdin) == NULL) break;

        msg[strcspn(msg, "\n")] = '\0';

        if (strcmp(msg, "/sair") == 0) {
            break;
        }

        send(sock, msg, strlen(msg), 0);
    }

    close(sock);
    return 0;
}
