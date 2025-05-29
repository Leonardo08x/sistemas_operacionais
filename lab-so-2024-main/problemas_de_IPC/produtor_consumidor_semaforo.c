/*
O problema do produtor e consumidor (também conhecido como o problema do buffer limitado), consiste em um conjunto de processos que compartilham um mesmo buffer.
Os processos chamados produtores põem informação no buffer. Os processos chamados consumidores retiram informação deste buffer. Quando o buffer estiver cheio, os
produtores devem aguardar por posições disponíveis para inserir os dados produzidos. Da mesma forma, quando o buffer estiver vazio os consumidores devem aguardar
por dados para serem consumidos.
*/

#include <stdio.h>
#include <stdlib.h>
#include <minix/mthread.h>
#include <unistd.h>

#define PR 1    // número de produtores
#define CN 1    // número de consumidores
#define N 5     // tamanho do buffer

void *produtor(void *meuid);
void *consumidor(void *meuid);

mthread_mutex_t mutex;
mthread_mutex_t empty;    // mutex simulando semáforo de posições livres
mthread_mutex_t full;     // mutex simulando semáforo de posições ocupadas
int empty_count = N;      // contador para simular semáforo empty
int full_count = 0;       // contador para simular semáforo full

int index_in = 0;         // índice de inserção
int index_out = 0;        // índice de remoção
int buffer[N] = {0};      // buffer inicializado com 0

int main(int argc, char *argv[]) {
  int erro;
  int i;
  int *id;

  mthread_mutex_init(&mutex, NULL);
  mthread_mutex_init(&empty, NULL);
  mthread_mutex_init(&full, NULL);
  mthread_mutex_unlock(&empty); // Inicialmente, empty está "liberado" (N posições livres)

  printf("BUFFER INICIALIZADO!\n");

  mthread_thread_t tPid[PR];

  for (i = 0; i < PR; i++) {
    id = (int *)malloc(sizeof(int));
    *id = i;
    erro = mthread_create(&tPid[i], NULL, (void *)produtor, *id);

    if (erro) {
      printf("erro na criacao do thread %d\n", i);
      exit(1);
    }
  }

  mthread_thread_t tCid[CN];

  for (i = 0; i < CN; i++) {
    id = (int *)malloc(sizeof(int));
    *id = i;
    erro = mthread_create(&tCid[i], NULL, (void *)consumidor, *id);

    if (erro) {
      printf("erro na criacao do thread %d\n", i);
      exit(1);
    }
  }

  mthread_join(tPid[0], NULL);
  return 0;
}

void *produtor(void *pi) {
  int item, i = (int)pi;

  while (1) {
    item = rand() % 5;     // produz dado

    mthread_mutex_lock(&mutex);
    while (empty_count == 0) { // simula sem_wait(&empty)
      mthread_mutex_unlock(&mutex);
      mthread_mutex_lock(&empty);
      mthread_mutex_lock(&mutex);
    }
    empty_count--;         // decrementa contador de posições livres

    buffer[index_in] = item;      // insere dado no buffer
    printf("Produtor %d está inserindo o item %d no buffer[%d]\n", i, item, index_in);
    index_in = (index_in + 1) % N; // cálculo do índice circular

    full_count++;          // incrementa contador de posições ocupadas
    if (full_count == N) {
      printf("BUFFER CHEIO!\n");
    }
    mthread_mutex_unlock(&full); // simula sem_post(&full)

    mthread_mutex_unlock(&mutex);
    sleep(rand() % 3);
  }
  return NULL;
}

void *consumidor(void *pi) {
  int item, i = (int)pi;

  while (1) {
    mthread_mutex_lock(&mutex);
    while (full_count == 0) { // simula sem_wait(&full)
      mthread_mutex_unlock(&mutex);
      mthread_mutex_lock(&full);
      mthread_mutex_lock(&mutex);
    }
    full_count--;          // decrementa contador de posições ocupadas

    item = buffer[index_out];     // remove dado do buffer
    printf("Consumidor %d está removendo o item %d do buffer[%d]\n", i, item, index_out);
    index_out = (index_out + 1) % N; // cálculo do índice circular

    empty_count++;         // incrementa contador de posições livres
    if (full_count == 0) {
      printf("BUFFER VAZIO!\n");
    }
    mthread_mutex_unlock(&empty); // simula sem_post(&empty)

    mthread_mutex_unlock(&mutex);
    sleep(rand() % 6);
  }
  return NULL;
}
