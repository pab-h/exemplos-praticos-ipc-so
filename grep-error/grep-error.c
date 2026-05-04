#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main() {
    int fd[2];
    pipe(fd);

    if (fork() == 0) {
        // Filho = filtro (grep ERROR)
        close(fd[1]);

        char buffer[256];
        ssize_t n;
        while ((n = read(fd[0], buffer, sizeof(buffer) - 1)) > 0) {
            buffer[n] = '\0';  
        
            if (strstr(buffer, "ERROR")) {
                printf("Filtrado: %s", buffer);
            }
            
        }
    } else {
        // Pai = produtor de logs
        close(fd[0]);

        char *logs[] = {
            "INFO Sistema OK\n",
            "ERROR Falha no disco\n",
            "INFO Reiniciando\n",
            "ERROR Timeout\n"
        };

        for (int i = 0; i < 4; i++) {
            write(fd[1], logs[i], strlen(logs[i]));
            sleep(1);
        }
    }
}
