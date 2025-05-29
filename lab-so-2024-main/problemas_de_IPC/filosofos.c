#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

#define N 5
#define LEFT(i) (i)
#define RIGHT(i) ((i + 1) % N)

const char *left_fork_files[N] = {
    "fork0.lock",
    "fork1.lock",
    "fork2.lock",
    "fork3.lock",
    "fork4.lock"
};

// Função para "pegar" o talher (cria arquivo lock)
void pick_fork(const char *fork_file) {
    int fd;
    while (1) {
        fd = open(fork_file, O_CREAT | O_EXCL, 0444);
        if (fd >= 0) {
            // Pegou o talher (lock criado)
            close(fd);
            break;
        }
        // Se não conseguiu criar o arquivo, espera e tenta de novo
        usleep(100000); // 100ms
    }
}

// Função para "soltar" o talher (remove arquivo lock)
void release_fork(const char *fork_file) {
    unlink(fork_file);
}

void philosopher(int id) {
    while (1) {
        printf("Filosofo %d pensando...\n", id);
        sleep(2);

        // Filósofo pega os talheres na mesma ordem para evitar deadlock
        if (id % 2 == 0) {
            pick_fork(left_fork_files[LEFT(id)]);
            pick_fork(left_fork_files[RIGHT(id)]);
        } else {
            pick_fork(left_fork_files[RIGHT(id)]);
            pick_fork(left_fork_files[LEFT(id)]);
        }

        printf("\tFilosofo %d comendo...\n", id);
        sleep(2);

        release_fork(left_fork_files[LEFT(id)]);
        release_fork(left_fork_files[RIGHT(id)]);

        printf("\tFilosofo %d terminou de comer...\n", id);
    }
}

int main() {
    pid_t pid;
    int i;

    // Limpar arquivos de lock antigos
    for (i = 0; i < N; i++) {
        unlink(left_fork_files[i]);
    }

    for (i = 0; i < N; i++) {
        pid = fork();
        if (pid == 0) {
            philosopher(i);
            exit(0);
        } else if (pid < 0) {
            perror("fork");
            exit(1);
        }
    }

    // Espera filhos (filósofos) terminarem (eles não terminam nesse loop infinito)
    for (i = 0; i < N; i++) {
        wait(NULL);
    }

    return 0;
}
