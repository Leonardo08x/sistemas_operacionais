// Marcella Pantarotto (13/0143880)

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

#define PR 1  // número de produtores
#define CN 1  // número de consumidores
#define N 5   // tamanho do buffer

void *produtor(void *meuid);
void *consumidor(void *meuid);
void print_buffer();
int produce_item();
void insert_data(int data, int i);
void remove_data(long int i);

int buffer[N] = {0};    // declarando e inicializando o vetor do buffer com 0 em todas as posições

mthread_mutex_t mutex;
mthread_cond_t produtor_cond;
mthread_cond_t consumidor_cond;

int count = 0;          // contador de posições
int index_insert = -1;   // índice do produtor
int index_remove = -1;   // índice do consumidor

int main(int argc, char *argv[]) {
  int erro;
  int i;
  int *id;

  printf("START ");
  print_buffer();

  mthread_thread_t tPid[PR];

  mthread_mutex_init(&mutex, NULL);
  mthread_cond_init(&produtor_cond, NULL);
  mthread_cond_init(&consumidor_cond, NULL);

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
  int item;
  int i = (int)pi;

  while (1) {
    sleep(rand() % 2);
    item = produce_item();

    mthread_mutex_lock(&mutex);   // produtor pega o lock do buffer
    while (count == N) {  // verifica se o buffer está cheio
      printf("Buffer está cheio!\n\n");
      mthread_cond_wait(&produtor_cond, &mutex);  // adormece o produtor
    }

    index_insert = (index_insert + 1) % N;    // cálculo do índice do array circular
    insert_data(item, index_insert);      // inserir dado do buffer
    printf("PRODUTOR %d está produzindo conteúdo\n", i);
    
    count += 1;
    print_buffer();
    
    if (count == 1) {  // verifica que o buffer não está vazio para acordar o consumidor
      printf("Acorda consumidor!\n\n");
      mthread_cond_signal(&consumidor_cond);  // como só tem 1 consumidor, não há problema usar signal
    } 
    mthread_mutex_unlock(&mutex);   // produtor solta o lock do buffer
    sleep(rand() % 2);
  }
  return NULL;
}

void *consumidor(void *pi) {
  int i = (int)pi;

  while (1) {
    sleep(rand() % 2);

    mthread_mutex_lock(&mutex);   // consumidor pega o lock do buffer
    while (count == 0) {    // verifica se o buffer está vazio
      printf("Buffer está vazio!\n\n");
      mthread_cond_wait(&consumidor_cond, &mutex);    // adormece o consumidor
    }
    index_remove = (index_remove + 1) % N;    // cálculo do índice do array circular
    remove_data(index_remove);    // remover dado do buffer
    printf("CONSUMIDOR %d está consumindo conteúdo\n", i);

    count -= 1;
    print_buffer();

    if (count == N - 1) {    // verifica que o buffer não está cheio para acordar o produtor
      printf("Acorda produtor!\n\n");
      mthread_cond_signal(&produtor_cond);    // como só tem 1 produtor, não há problema usar signal
    }
    mthread_mutex_unlock(&mutex);   // consumidor solta o lock do buffer
    sleep((rand() % 5) + 1);
  }
  return NULL;
}

void print_buffer() {
  printf("BUFFER:\n");
  for (size_t i = 0; i < N; i++) {
    printf("buffer[%ld] = %d\n", i, buffer[i]);
  }
  printf("\n");
}

int produce_item() {
  return 1;
}

void insert_data(int data, int index) {
  buffer[index] = data;
}

void remove_data(long int index) {
  buffer[index] = 0;
}
