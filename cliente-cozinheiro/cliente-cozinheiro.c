#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <time.h>

#define MAX_TEXT 100

struct mensagem {
    long tipo;
    char texto[MAX_TEXT];
};

void cliente(int msgid, int id) {
    struct mensagem msg;
    msg.tipo = 1;

    for (int i = 0; i < 3; i++) {
        snprintf(msg.texto, MAX_TEXT, "Cliente %d pediu item %d", id, i);
        msgsnd(msgid, &msg, sizeof(msg.texto), 0);

        usleep((rand() % 1000) * 1000);
    }

    exit(0);
}

void cozinheiro(int msgid) {
    struct mensagem msg;

    while (1) {
        msgrcv(msgid, &msg, sizeof(msg.texto), 0, 0);

        if (strcmp(msg.texto, "FIM") == 0)
            break;

        printf("Preparando: %s\n", msg.texto);
        sleep(1);
    }

    exit(0);
}

int main() {
    srand(time(NULL));

    key_t key = ftok("fila", 65);
    int msgid = msgget(key, 0666 | IPC_CREAT);

    if (msgid == -1) {
        perror("msgget");
        exit(1);
    }

    pid_t pid;

    // Cozinheiro
    pid = fork();
    if (pid == 0) {
        cozinheiro(msgid);
    }

    // Clientes
    for (int i = 0; i < 3; i++) {
        pid = fork();
        if (pid == 0) {
            cliente(msgid, i);
        }
    }

    // Espera clientes
    for (int i = 0; i < 3; i++) {
        wait(NULL);
    }

    // Envia mensagem de término
    struct mensagem fim;
    fim.tipo = 1;
    strcpy(fim.texto, "FIM");

    msgsnd(msgid, &fim, sizeof(fim.texto), 0);

    wait(NULL); // espera cozinheiro

    // Remove fila
    msgctl(msgid, IPC_RMID, NULL);

    return 0;
}