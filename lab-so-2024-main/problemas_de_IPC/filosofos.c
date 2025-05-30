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

int estados[N];              // Array para armazenar o estado de cada filósofo
mthread_mutex_t mutex;       // Mutex único para proteger todas as operações
int filosofo_ativo = 0;      // Controla qual filósofo está ativo (0 a 4)

void *filosofos(void *arg);
void tenta_comer(int n);
void pega_talheres(int n);
void devolve_talheres(int n);

int main() {
    int i;
    int *ids[N];

    srand(time(NULL));       // Inicializa a semente para números aleatórios
    mthread_mutex_init(&mutex, NULL);

    // Inicializa os estados
    for (i = 0; i < N; i++) {
        estados[i] = PENSANDO; // Inicializa todos os filósofos como PENSANDO
    }

    mthread_thread_t threads[N];

    // Cria as threads dos filósofos
    for (i = 0; i < N; i++) {
        ids[i] = malloc(sizeof(int));
        *ids[i] = i;
        if (mthread_create(&threads[i], NULL, filosofos, (void *)ids[i]) == 0) {
            printf("Thread do filósofo %d criada com sucesso.\n", i);
        } else {
            printf("Erro ao criar thread do filósofo %d.\n", i);
            exit(1);
        }
    }

    // Espera todas as threads
    for (i = 0; i < N; i++) {
        mthread_join(threads[i], NULL);
        free(ids[i]); // Libera memória após a thread terminar
    }

    mthread_mutex_destroy(&mutex);
    return 0;
}

void *filosofos(void *arg) {
    int n = *((int *)arg); // Extrai o ID do filósofo

    while (1) {
        // Pensar
        printf("Filósofo %d pensando...\n", n);
        sleep(1 + rand() % 3);

        // Tentar comer
        mthread_mutex_lock(&mutex);
        estados[n] = FAMINTO;
        printf("Filósofo %d está faminto.\n", n);
        tenta_comer(n);
        mthread_mutex_unlock(&mutex);

        // Aguarda até ser o filósofo ativo e poder comer
        while (estados[n] != COMENDO) {
            sleep(1); // Pausa para dar chance a outras threads
            mthread_mutex_lock(&mutex);
            tenta_comer(n);
            mthread_mutex_unlock(&mutex);
        }

        // Comer
        printf("\tFilósofo %d COMENDO...\n", n);
        sleep(1 + rand() % 3);

        // Liberar talheres
        printf("\tFilósofo %d terminou de comer.\n", n);
        mthread_mutex_lock(&mutex);
        devolve_talheres(n);
        mthread_mutex_unlock(&mutex);
    }
    return NULL;
}

void tenta_comer(int n) {
    // Verifica se é o filósofo ativo e se os vizinhos não estão comendo
    if (estados[n] == FAMINTO &&
        estados[ESQ(n)] != COMENDO && estados[DIR(n)] != COMENDO &&
        filosofo_ativo == n) {
        estados[n] = COMENDO;
        pega_talheres(n);
        filosofo_ativo = (filosofo_ativo + 1) % N; // Passa a vez para o próximo filósofo
    }
}

void pega_talheres(int n) {
    // Simula pegar os talheres (apenas atualiza o estado, protegido pelo mutex)
    printf("\tFilósofo %d pegou talher %d e talher %d\n", n, ESQ(n), DIR(n));
}

void devolve_talheres(int n) {
    // Simula devolver os talheres e atualiza o estado
    estados[n] = PENSANDO;
    printf("\tFilósofo %d voltou a pensar.\n", n);
    // Verifica se os vizinhos podem comer
    tenta_comer(ESQ(n));
    tenta_comer(DIR(n));
}
