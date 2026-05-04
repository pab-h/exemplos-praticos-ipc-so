#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <time.h>

int main() {
    // Criar memória compartilhada para 2 inteiros (CPU, RAM)
    int *shared = mmap(NULL, 2 * sizeof(int),
                       PROT_READ | PROT_WRITE,
                       MAP_SHARED | MAP_ANONYMOUS,
                       -1, 0);

    if (shared == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    pid_t pid1 = fork();

    if (pid1 == 0) {
        // Processo sensor
        srand(time(NULL) ^ getpid());

        while (1) {
            shared[0] = rand() % 101; // CPU
            shared[1] = rand() % 101; // RAM
            sleep(1);
        }
    }

    pid_t pid2 = fork();

    if (pid2 == 0) {
        // Processo dashboard
        while (1) {
            printf("CPU: %d%% | RAM: %d%%\n", shared[0], shared[1]);
            sleep(1);
        }
    }

    // Processo pai espera 10 segundos
    sleep(10);

    return 0;
}
