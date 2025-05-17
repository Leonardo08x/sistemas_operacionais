#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    int n;
    printf("Insira o número de execuções: ");
    scanf("%d", &n);

    for (int i = 0; i < n; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            printf("Execução do processo filho\n");
            execl("./hello", "hello", NULL);
            perror("execl failed");
            exit(1);
        } else if (pid > 0) {
            printf("Execução do processo pai\n");
        } else {
            perror("fork failed");
        }
    }

    return 0;
}
