#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define MSG_KEY 1234

struct mensagem {
    long tipo;
    char texto[100];
};

int main() {

    int msgid;

    // Cria a fila de mensagens
    msgid = msgget(MSG_KEY, 0666 | IPC_CREAT);

    if (msgid == -1) {
        perror("msgget");
        return 1;
    }

    pid_t pid = fork();

    // Processo filho -> RECEIVER
    if (pid == 0) {

        struct mensagem msg_recebida;

        // Recebe mensagem
        msgrcv(
            msgid,
            &msg_recebida,
            sizeof(msg_recebida.texto),
            1,
            0
        );

        printf("\n[RECEIVER]\n");
        printf("Mensagem recebida: %s\n", msg_recebida.texto);

    }
    // Processo pai -> SENDER
    else {

        struct mensagem msg_enviada;

        msg_enviada.tipo = 1;

        strcpy(
            msg_enviada.texto,
            "Olá! Mensagem enviada via fila!"
        );

        sleep(1);

        // Envia mensagem
        msgsnd(
            msgid,
            &msg_enviada,
            sizeof(msg_enviada.texto),
            0
        );

        printf("\n[SENDER]\n");
        printf("Mensagem enviada: %s\n", msg_enviada.texto);

        wait(NULL);

        // Remove a fila
        msgctl(msgid, IPC_RMID, NULL);
    }

    return 0;
}