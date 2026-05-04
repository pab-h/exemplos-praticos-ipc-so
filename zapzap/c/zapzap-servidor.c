#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 9000
#define MAX_CLIENTS 100
#define BUFFER_SIZE 1024

int clients[MAX_CLIENTS];
int client_count = 0;

pthread_mutex_t lock;

// Enviar mensagem para todos (exceto quem enviou)
void broadcast(char *msg, int sender_sock) {
    pthread_mutex_lock(&lock);

    for (int i = 0; i < client_count; i++) {
        if (clients[i] != sender_sock) {
            if (send(clients[i], msg, strlen(msg), 0) < 0) {
                perror("Erro ao enviar");
            }
        }
    }

    pthread_mutex_unlock(&lock);
}

// Remover cliente da lista
void remove_client(int sock) {
    pthread_mutex_lock(&lock);

    for (int i = 0; i < client_count; i++) {
        if (clients[i] == sock) {
            for (int j = i; j < client_count - 1; j++) {
                clients[j] = clients[j + 1];
            }
            client_count--;
            break;
        }
    }

    pthread_mutex_unlock(&lock);
}

// Thread para lidar com cliente
void* handle_client(void* arg) {
    int client_sock = *(int*)arg;
    free(arg);

    char buffer[BUFFER_SIZE];
    char nome[BUFFER_SIZE];

    // Receber nome
    int bytes = recv(client_sock, nome, BUFFER_SIZE - 1, 0);
    if (bytes <= 0) {
        close(client_sock);
        return NULL;
    }
    nome[bytes] = '\0';

    printf("[+] Cliente entrou: %s\n", nome);

    char entrada_msg[BUFFER_SIZE];
    snprintf(entrada_msg, BUFFER_SIZE, "[%s entrou no chat]", nome);
    broadcast(entrada_msg, client_sock);

    while (1) {
        bytes = recv(client_sock, buffer, BUFFER_SIZE - 1, 0);

        if (bytes <= 0) {
            break;
        }

        buffer[bytes] = '\0';

        char mensagem_formatada[BUFFER_SIZE];
        snprintf(mensagem_formatada, BUFFER_SIZE, "%s: %s", nome, buffer);

        printf("%s\n", mensagem_formatada);

        broadcast(mensagem_formatada, client_sock);
    }

    printf("[-] Cliente saiu: %s\n", nome);

    remove_client(client_sock);
    close(client_sock);

    return NULL;
}

int main() {
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    pthread_mutex_init(&lock, NULL);

    // Criar socket
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        perror("Erro ao criar socket");
        return 1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY; // 0.0.0.0
    server_addr.sin_port = htons(PORT);

    // Bind
    if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Erro no bind");
        return 1;
    }

    // Listen
    if (listen(server_sock, 10) < 0) {
        perror("Erro no listen");
        return 1;
    }

    printf("Servidor rodando em 0.0.0.0:%d\n", PORT);

    while (1) {
        client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &addr_len);
        if (client_sock < 0) {
            perror("Erro no accept");
            continue;
        }

        pthread_mutex_lock(&lock);

        if (client_count < MAX_CLIENTS) {
            clients[client_count++] = client_sock;

            int *pclient = malloc(sizeof(int));
            *pclient = client_sock;

            pthread_t thread;
            pthread_create(&thread, NULL, handle_client, pclient);
            pthread_detach(thread);
        } else {
            printf("Máximo de clientes atingido.\n");
            close(client_sock);
        }

        pthread_mutex_unlock(&lock);
    }

    close(server_sock);
    pthread_mutex_destroy(&lock);

    return 0;
}
