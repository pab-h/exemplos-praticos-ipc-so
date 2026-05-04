#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>

#define MSG_SIZE 100

void cliente(int write_fd, int id) {
    char buffer[MSG_SIZE];

    srand(getpid());

    for (int i = 0; i < 3; i++) {
        snprintf(buffer, MSG_SIZE, "Cliente %d pediu item %d", id, i);
        write(write_fd, buffer, MSG_SIZE);

        usleep((rand() % 1000) * 1000); // sleep aleatório (ms)
    }

    exit(0);
}

void cozinheiro(int read_fd) {
    char buffer[MSG_SIZE];

    while (1) {
        read(read_fd, buffer, MSG_SIZE);

        if (strcmp(buffer, "END") == 0)
            break;

        printf("Preparando: %s\n", buffer);
        fflush(stdout);

        sleep(1);
    }

    exit(0);
}

int main() {
    int pipefd[2];
    pipe(pipefd);

    int read_fd = pipefd[0];
    int write_fd = pipefd[1];

    // cria cozinheiro
    pid_t coz = fork();
    if (coz == 0) {
        close(write_fd); // não escreve
        cozinheiro(read_fd);
    }

    // cria clientes
    pid_t clientes[3];
    for (int i = 0; i < 3; i++) {
        clientes[i] = fork();

        if (clientes[i] == 0) {
            close(read_fd); // não lê
            cliente(write_fd, i);
        }
    }

    // processo principal
    close(read_fd); // não lê

    // espera clientes (join)
    for (int i = 0; i < 3; i++) {
        waitpid(clientes[i], NULL, 0);
    }

    // envia sinal de parada
    char end_msg[MSG_SIZE] = "END";
    write(write_fd, end_msg, MSG_SIZE);

    close(write_fd);

    // espera cozinheiro
    waitpid(coz, NULL, 0);

    return 0;
}
