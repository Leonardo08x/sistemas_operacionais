/*
The problem is defined as follows: There are 5 philosophers sitting at a round table. Between each adjacent pair of philosophers is a chopstick.
In other words, there are five chopsticks. Each philosopher does two things: think and eat. The philosopher thinks for a while, and then stops
thinking and becomes hungry. When the philosopher becomes hungry, he/she cannot eat until he/she owns the chopsticks to his/her left and right.
When the philosopher is done eating he/she puts down the chopsticks and begins thinking again.
*/

// Adaptado para Minix com mthread

#include <stdlib.h>
#include <stdio.h>
#include <minix/mthread.h>
#include <time.h>
#include <unistd.h>

#define N 5              // quantidade de talheres e filósofos
#define ESQ(id) (id)     // talher da esquerda
#define DIR(id) (id+1)%N // talher da direita

#define PENSANDO 0
#define FAMINTO 1
#define COMENDO 2
int estados[N];

mthread_mutex_t mutex; // mutex principal para sincronização
mthread_mutex_t s[N];  // mutexes para simular semáforos dos talheres

void *filosofos(void *arg);
void pega_talher(int n);
void devolve_talher(int n);
void comer(int n);

int main() {
    int i;
    int *id;

    // Inicializar mutex principal
    mthread_mutex_init(&mutex, NULL);

    // Inicializar mutexes para os talheres (simulando semáforos)
    for (i = 0; i < N; i++) {
        mthread_mutex_init(&s[i], NULL);
        mthread_mutex_unlock(&s[i]); // Inicialmente desbloqueado (equivalente a semáforo com valor 1)
    }

    mthread_thread_t r[N];

    // Criação das threads de filósofos
    for (i = 0; i < N; i++) {
        id = (int *)malloc(sizeof(int));
        *id = i;
        mthread_create(&r[i], NULL, (void *)filosofos, *id);
    }

    mthread_join(r[0], NULL);
    return 0;
}

void *filosofos(void *arg) {
    int n = (int)arg; // mthread passa argumento diretamente como int
    while (1) {
        // Pensar
        printf("Filosofo %d pensando ...\n", n);
        sleep(3);

        pega_talher(n);
        // Comer
        printf("\tFilosofo %d comendo ...\n", n);
        sleep(3);

        printf("\tFilosofo %d acabou de comer ...\n", n);
        devolve_talher(n);
    }
    return NULL;
}

void pega_talher(int n) {
    mthread_mutex_lock(&mutex); // Pega lock para mudar estado
    estados[n] = FAMINTO;
    comer(n);
    mthread_mutex_unlock(&mutex); // Libera lock
    mthread_mutex_lock(&s[n]);   // Bloqueia para simular sem_wait
}

void devolve_talher(int n) {
    mthread_mutex_lock(&mutex); // Pega lock para mudar estado
    estados[n] = PENSANDO;
    comer(ESQ(n)); // Verifica se o vizinho à esquerda pode comer
    comer(DIR(n)); // Verifica se o vizinho à direita pode comer
    mthread_mutex_unlock(&mutex); // Libera lock
    mthread_mutex_unlock(&s[n]);  // Libera mutex do talher
}

void comer(int n) {
    // Testar se pode comer
    if (estados[n] == FAMINTO && estados[ESQ(n)] != COMENDO && estados[DIR(n)] != COMENDO) {
        estados[n] = COMENDO;
        mthread_mutex_unlock(&s[n]); // Simula sem_post, liberando o talher
    }
}
