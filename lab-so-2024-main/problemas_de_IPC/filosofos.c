#include <stdlib.h>
#include <stdio.h>
#include <minix/mthread.h>
#include <time.h>
#include <unistd.h>

#define N 5              // Quantidade de filósofos e talheres
#define ESQ(id) (id)     // Talher da esquerda
#define DIR(id) ((id + 1) % N) // Talher da direita

#define PENSANDO 0
#define FAMINTO 1
#define COMENDO 2

int estados[N];
mthread_mutex_t mutex;       // Mutex para proteger estados[]
mthread_mutex_t talheres[N]; // Mutexes para os talheres

void *filosofos(void *arg);
void pega_talher(int n);
void devolve_talher(int n);
void verificar_vizinhos(int n);

int main() {
    int i;
    int *ids[N];

    // Inicializar mutex principal
    mthread_mutex_init(&mutex, NULL);

    // Inicializar mutexes para os talheres
    for (i = 0; i < N; i++) {
        mthread_mutex_init(&talheres[i], NULL);
    }

    mthread_thread_t threads[N];

    // Criar threads para os filósofos
    for (i = 0; i < N; i++) {
        ids[i] = malloc(sizeof(int));
        *ids[i] = i;
        mthread_create(&threads[i], NULL, filosofos, (void *)ids[i]);
    }

    // Esperar todas as threads terminarem (embora sejam loops infinitos)
    for (i = 0; i < N; i++) {
        mthread_join(threads[i], NULL);
    }

    return 0;
}

void *filosofos(void *arg) {
    int n = *((int *)arg);
    free(arg); // Liberar memória alocada para o ID

    while (1) {
        // Pensar
        printf("Filosofo %d pensando...\n", n);
        sleep(1 + rand() % 3);

        pega_talher(n);

        // Comer
        printf("\tFilosofo %d COMENDO...\n", n);
        sleep(1 + rand() % 3);

        printf("\tFilosofo %d terminou de comer.\n", n);
        devolve_talher(n);
    }
    return NULL;
}

void pega_talher(int n) {
    mthread_mutex_lock(&mutex);
    estados[n] = FAMINTO;
    verificar_vizinhos(n); // Verifica se pode pegar os talheres
    mthread_mutex_unlock(&mutex);

    // Se não puder comer, bloqueia no talher[n] (semáforo implícito)
    mthread_mutex_lock(&talheres[n]);
}

void devolve_talher(int n) {
    mthread_mutex_lock(&mutex);
    estados[n] = PENSANDO;
    verificar_vizinhos(ESQ(n)); // Verifica se o vizinho esquerdo pode comer
    verificar_vizinhos(DIR(n)); // Verifica se o vizinho direito pode comer
    mthread_mutex_unlock(&mutex);
}

void verificar_vizinhos(int n) {
    if (estados[n] == FAMINTO && estados[ESQ(n)] != COMENDO && estados[DIR(n)] != COMENDO) {
        estados[n] = COMENDO;
        mthread_mutex_unlock(&talheres[n]); // Libera o "semáforo" para o filósofo comer
    }
}
