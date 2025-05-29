#include <stdlib.h>
#include <stdio.h>
#include <minix/mthread.h>
#include <time.h>
#include <unistd.h>

#define N 5                  // Número de filósofos/talheres
#define ESQ(id) (id)         // Índice do talher da esquerda
#define DIR(id) ((id + 1) % N) // Índice do talher da direita

#define PENSANDO 0
#define FAMINTO 1
#define COMENDO 2

int estados[N];
mthread_mutex_t mutex;       // Protege o array `estados`
mthread_mutex_t talheres[N]; // Mutexes para os talheres

void *filosofos(void *arg);
void pega_talheres(int n);
void devolve_talheres(int n);

int main() {
    int i;
    int *ids[N];

    mthread_mutex_init(&mutex, NULL);

    // Inicializa os talheres (mutexes)
    for (i = 0; i < N; i++) {
        mthread_mutex_init(&talheres[i], NULL);
    }

    mthread_thread_t threads[N];

    // Cria as threads dos filósofos
    for (i = 0; i < N; i++) {
        ids[i] = malloc(sizeof(int));
        *ids[i] = i;
        mthread_create(&threads[i], NULL, filosofos, (void *)ids[i]);
    }

    // Espera todas as threads (loop infinito)
    for (i = 0; i < N; i++) {
        mthread_join(threads[i], NULL);
    }

    return 0;
}

void *filosofos(void *arg) {
    int n = *((int *)arg);
    free(arg); // Libera o ID alocado

    while (1) {
        // Pensar
        printf("Filosofo %d pensando...\n", n);
        sleep(1 + rand() % 3);

        // Pegar talheres e comer
        pega_talheres(n);
        printf("\tFilosofo %d COMENDO...\n", n);
        sleep(1 + rand() % 3);

        // Liberar talheres
        printf("\tFilosofo %d terminou de comer.\n", n);
        devolve_talheres(n);
    }
    return NULL;
}

void pega_talheres(int n) {
    // Define a ordem para pegar os talheres (evita deadlock)
    int primeiro = ESQ(n) < DIR(n) ? ESQ(n) : DIR(n);
    int segundo = ESQ(n) < DIR(n) ? DIR(n) : ESQ(n);

    // Pega o primeiro talher (sempre em ordem crescente de ID)
    mthread_mutex_lock(&talheres[primeiro]);
    printf("\tFilosofo %d pegou talher %d (esquerda)\n", n, primeiro);

    // Pega o segundo talher
    mthread_mutex_lock(&talheres[segundo]);
    printf("\tFilosofo %d pegou talher %d (direita)\n", n, segundo);

    // Atualiza o estado para COMENDO
    mthread_mutex_lock(&mutex);
    estados[n] = COMENDO;
    mthread_mutex_unlock(&mutex);
}

void devolve_talheres(int n) {
    // Libera os talheres (ordem inversa não é necessária)
    mthread_mutex_unlock(&talheres[ESQ(n)]);
    mthread_mutex_unlock(&talheres[DIR(n)]);

    // Atualiza o estado para PENSANDO
    mthread_mutex_lock(&mutex);
    estados[n] = PENSANDO;
    mthread_mutex_unlock(&mutex);
}
