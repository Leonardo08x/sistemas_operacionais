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
mthread_mutex_t mutex;       // Protege o array `estados`
mthread_mutex_t talheres[N]; // Mutexes para os talheres

void *filosofos(void *arg);
void pega_talheres(int n);
void devolve_talheres(int n);
void testa_comer(int n);     // Nova função para verificar se o filósofo pode comer

int main() {
    int i;
    int *ids[N];

    srand(time(NULL));       // Inicializa a semente para números aleatórios
    mthread_mutex_init(&mutex, NULL);

    // Inicializa os talheres (mutexes)
    for (i = 0; i < N; i++) {
        estados[i] = PENSANDO; // Inicializa todos os filósofos como PENSANDO
        mthread_mutex_init(&talheres[i], NULL);
    }

    mthread_thread_t threads[N];

    // Cria as threads dos filósofos
    for (i = 0; i < N; i++) {
        ids[i] = malloc(sizeof(int));
        *ids[i] = i;
        if (mthread_create(&threads[i], NULL, filosofos, (void *)ids[i]) == 0) {
            printf("Thread do filósofo %d criada com sucesso.\n", i); // Log de criação
        } else {
            printf("Erro ao criar thread do filósofo %d.\n", i);
            exit(1);
        }
    }

    // Espera todas as threads (loop infinito)
    for (i = 0; i < N; i++) {
        mthread_join(threads[i], NULL);
        free(ids[i]); // Libera memória após a thread terminar
    }

    return 0;
}

void *filosofos(void *arg) {
    int n = *((int *)arg); // Não libera arg aqui, será liberado no main

    while (1) {
        // Pensar
        printf("Filósofo %d pensando...\n", n);
        sleep(1 + rand() % 3);

        // Tentar comer
        mthread_mutex_lock(&mutex);
        estados[n] = FAMINTO; // Marca como faminto
        printf("Filósofo %d está faminto.\n", n);
        testa_comer(n);       // Verifica se pode comer
        mthread_mutex_unlock(&mutex);

        // Aguarda até poder comer (se necessário)
        mthread_mutex_lock(&talheres[n]); // Usa o mutex do filósofo como proxy para espera
        mthread_mutex_unlock(&talheres[n]);

        // Comer
        printf("\tFilósofo %d COMENDO...\n", n);
        sleep(1 + rand() % 3);

        // Liberar talheres
        printf("\tFilósofo %d terminou de comer.\n", n);
        devolve_talheres(n);
    }
    return NULL;
}

void pega_talheres(int n) {
    // Define a ordem para pegar os talheres (evita deadlock)
    int primeiro = ESQ(n) < DIR(n) ? ESQ(n) : DIR(n);
    int segundo = ESQ(n) < DIR(n) ? DIR(n) : ESQ(n);

    // Pega o primeiro talher
    mthread_mutex_lock(&talheres[primeiro]);
    printf("\tFilósofo %d pegou talher %d\n", n, primeiro);

    // Pega o segundo talher
    mthread_mutex_lock(&talheres[segundo]);
    printf("\tFilósofo %d pegou talher %d\n", n, segundo);
}

void devolve_talheres(int n) {
    // Libera os talheres
    mthread_mutex_unlock(&talheres[ESQ(n)]);
    mthread_mutex_unlock(&talheres[DIR(n)]);

    // Atualiza o estado para PENSANDO e verifica vizinhos
    mthread_mutex_lock(&mutex);
    estados[n] = PENSANDO;
    printf("\tFilósofo %d voltou a pensar.\n", n);
    testa_comer(ESQ(n)); // Verifica se o vizinho à esquerda pode comer
    testa_comer(DIR(n)); // Verifica se o vizinho à direita pode comer
    mthread_mutex_unlock(&mutex);
}

void testa_comer(int n) {
    // Verifica se o filósofo pode comer
    if (estados[n] == FAMINTO && estados[ESQ(n)] != COMENDO && estados[DIR(n)] != COMENDO) {
        estados[n] = COMENDO;
        pega_talheres(n); // Pega os talheres se puder comer
    }
}
