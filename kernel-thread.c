#include <linux/module.h>    // incluído para todos os módulos do kernel
#include <linux/init.h>      // incluído para macros init e exit
#include <linux/kthread.h>   // incluído para funções relacionadas a threads
#include <linux/sched.h>     // incluído para criar task_struct
#include <linux/delay.h>     // incluído para a função de sleep/delay na thread

// array para task_struct para armazenar informações das tarefas
static struct task_struct *ktharr[4];

// função de longa execução a ser executada dentro de uma thread, rodará por 30 segundos
int thread_function(void *idx) {
    unsigned int i = 0;
    int tid = *(int *)idx;

    // chamada kthread_should_stop é importante
    while (!kthread_should_stop()) {
        printk(KERN_INFO "Thread %d (PID: %d) Ainda executando...! %d segundos\n", tid, task_pid_nr(current), i);
        i++;
        if (i == 30)
            break;
        msleep(1000);
    }
    printk(KERN_INFO "Thread %d (PID: %d) parada\n", tid, task_pid_nr(current));
    return 0;
}

// inicializa uma thread por vez
int initialize_thread(struct task_struct **kth, int idx) {
    char th_name[20];
    sprintf(th_name, "kthread%d", idx);
    *kth = kthread_create(thread_function, &idx, (const char *)th_name);
    if (*kth != NULL) {
        wake_up_process(*kth);
        printk(KERN_INFO "%s (PID: %d) está executando\n", th_name, task_pid_nr(*kth));
    } else {
        printk(KERN_INFO "kthread %s não pôde ser criada\n", th_name);
        return -1;
    }
    return 0;
}

// função de inicialização do módulo
static int __init mod_init(void) {
    int i = 0;
    printk(KERN_INFO "Inicializando módulo de threads\n");
    for (i = 0; i < 4; i++) {
        // inicializa uma thread por vez
        if (initialize_thread(&ktharr[i], i) == -1) {
            return -1;
        }
    }
    printk(KERN_INFO "Todas as threads estão executando\n");
    return 0;
}

// função de saída do módulo
static void __exit mod_exit(void) {
    int i = 0;
    int ret = 0;
    printk(KERN_INFO "Saindo do módulo de threads\n");
    for (i = 0; i < 4; i++) {
        // para todas as threads antes de remover o módulo
        ret = kthread_stop(ktharr[i]);
        if (ret) {  // kthread_stop retorna 0 em sucesso, diferente de 0 em falha
            printk(KERN_INFO "Não foi possível parar a thread %d\n", i);
        }
    }
    printk(KERN_INFO "Todas as threads foram paradas\n");
}

MODULE_LICENSE("GPL");
module_init(mod_init);
module_exit(mod_exit);