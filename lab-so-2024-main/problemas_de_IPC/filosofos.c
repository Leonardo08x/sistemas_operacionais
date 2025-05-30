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
mthread_mutex_t mutex;       // Mutex global para proteger estados e talheres disponíveis
int talheres_disponiveis[N] = {1, 1, 1, 1, 1}; // 1 = talher disponível, 0 = talher ocupado
int proximo_filosofo = 0;    // Controla qual filósofo tem prioridade para tentar comer

void *filosofos(void *arg);
void pega_talheres(int n);
void devolve_talheres(int n);
void testa_comer(int n);

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
        estados[n] = FAMINTO; // Marca como faminto
        printf("Filósofo %d está faminto.\n", n);
        testa_comer(n);       // Verifica se pode comer
        mthread_mutex_unlock(&mutex);

        // Aguarda até poder comer
        while (estados[n] != COMENDO) {
            sleep(1); // Pausa para evitar busy-waiting e dar chance a outras threads
            mthread_mutex_lock(&mutex);
            testa_comer(n);
            mthread_mutex_unlock(&mutex);
        }

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
    // Marca os talheres como ocupados
    talheres_disponiveis[ESQ(n)] = 0;
    talheres_disponiveis[DIR(n)] = 0;
    printf("\tFilósofo %d pegou talher %d e talher %d\n", n, ESQ(n), DIR(n));
}

void devolve_talheres(int n) {
    // Libera os talheres
    talheres_disponiveis[ESQ(n)] = 1;
    talheres_disponiveis[DIR(n)] = 1;

    // Atualiza o estado para PENSANDO e verifica vizinhos
    estados[n] = PENSANDO;
    printf("\tFilósofo %d voltou a pensar.\n", n);
    proximo_filosofo = (proximo_filosofo + 1) % N; // Dá prioridade ao próximo filósofo
    testa_comer(ESQ(n)); // Verifica se o vizinho à esquerda pode comer
    testa_comer(DIR(n)); // Verifica se o vizinho à direita pode comer
}

void testa_comer(int n) {
    // Verifica se o filósofo pode comer
    if (estados[n] == FAMINTO && 
        estados[ESQ(n)] != COMENDO && estados[DIR(n)] != COMENDO &&
        talheres_disponiveis[ESQ(n)] && talheres_disponiveis[DIR(n)] &&
        proximo_filosofo == n) {
        estados[n] = COMENDO;
        pega_talheres(n); // Pega os talheres
    }
}
