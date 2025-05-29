/*
Este problema é uma abstração do acesso à base de dados, onde não existe o perigo em termos diversos processos lendo concorrentemente (leitores), mas escrevendo ou 
mudando os dados (escritores) deve ser feito sob exclusão mútua para garantir consistência. Leitores e escritores é uma família de modelos de controle de concorrência 
em que leitores (entes que não alteram conteúdo) podem acessar concorrentemente os recursos (por exemplo, um banco de dados) e escritores (entes que alteram conteúdo) 
requerem acesso exclusivo. Este código resolve o problema de starvation dos escritores.
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <minix/mthread.h>

#define TRUE 1
#define NE 10 // número de escritores
#define NL 10 // número de leitores

mthread_mutex_t lock_bd;  // lock do banco de dados
mthread_mutex_t lock_nl;  // lock de leitura
mthread_mutex_t lock_vez; // lock da vez (competição entre leitores e escritores)
int num_leitores = 0;     // contador de leitores acessando o banco de dados
int escritores_esperando = 0; // contador de escritores esperando (para evitar starvation)

void *reader(void *arg); // thread de execução dos leitores
void *writer(void *arg); // thread de execução dos escritores
void read_data_base(int i); // função usada pelo leitor para ler um dado
void use_data_read(int i);  // função usada pelo leitor para usar o dado lido
void think_up_data(int i);  // função usada pelo escritor para produzir um dado
void write_data_base(int i); // função usada pelo escritor para escrever o dado

int main() {
    mthread_thread_t r[NL], w[NE]; // vetores para leitores e escritores
    int i;
    int *id;

    // Inicializar mutexes
    mthread_mutex_init(&lock_bd, NULL);
    mthread_mutex_init(&lock_nl, NULL);
    mthread_mutex_init(&lock_vez, NULL);

    // Criando leitores
    for (i = 0; i < NL; i++) {
        id = (int *)malloc(sizeof(int));
        *id = i;
        mthread_create(&r[i], NULL, (void *)reader, *id);
    }

    // Criando escritores
    for (i = 0; i < NE; i++) {
        id = (int *)malloc(sizeof(int));
        *id = i;
        mthread_create(&w[i], NULL, (void *)writer, *id);
    }

    mthread_join(r[0], NULL); // espera por um leitor para manter a main bloqueada
    return 0;
}

void *reader(void *arg) {
    int i = (int)arg; // mthread passa argumento como int
    while (TRUE) {
        // Verifica se há escritores esperando antes de tentar ler
        mthread_mutex_lock(&lock_vez);
        if (escritores_esperando > 0) {
            mthread_mutex_unlock(&lock_vez);
            usleep(100000); // Espera brevemente para dar chance aos escritores
            continue; // Tenta novamente
        }

        mthread_mutex_lock(&lock_nl);
        num_leitores++;
        if (num_leitores == 1) {
            mthread_mutex_lock(&lock_bd); // Primeiro leitor bloqueia o banco
        }
        mthread_mutex_unlock(&lock_vez);
        mthread_mutex_unlock(&lock_nl);

        read_data_base(i); // Acessa região crítica

        mthread_mutex_lock(&lock_nl);
        num_leitores--;
        if (num_leitores == 0) {
            mthread_mutex_unlock(&lock_bd); // Último leitor libera o banco
        }
        mthread_mutex_unlock(&lock_nl);

        use_data_read(i); // Região não crítica
    }
    return NULL;
}

void *writer(void *arg) {
    int i = (int)arg;
    while (TRUE) {
        think_up_data(i); // Região não crítica

        mthread_mutex_lock(&lock_vez);
        escritores_esperando++; // Sinaliza que um escritor está esperando
        mthread_mutex_lock(&lock_bd); // Bloqueia o banco de dados
        escritores_esperando--; // Escritor conseguiu acesso, decrementa
        write_data_base(i); // Atualiza os dados
        mthread_mutex_unlock(&lock_vez);
        mthread_mutex_unlock(&lock_bd); // Libera o banco de dados
    }
    return NULL;
}

void read_data_base(int i) {
    printf("Leitor %d está lendo os dados! Número de leitores: %d\n", i, num_leitores);
    sleep(rand() % 5);
}

void use_data_read(int i) {
    printf("Leitor %d está usando os dados lidos! Número de leitores: %d\n", i, num_leitores);
    sleep(rand() % 5);
}

void think_up_data(int i) {
    printf("Escritor %d está pensando no que escrever!\n", i);
    sleep(rand() % 5);
}

void write_data_base(int i) {
    printf("Escritor %d está escrevendo os dados! Número de leitores: %d\n", i, num_leitores);
    sleep(rand() % 5 + 15);
}
