/*
 * trecho3_mutex_vazado.c
 * Bug: sem_post esquecido em um caminho de saida antecipada (lock vazado).
 *
 * Compilar: gcc -Wall -Wextra -pthread trecho3_mutex_vazado.c -o trecho3
 * Rodar:    ./trecho3
 *
 * O primeiro pedido processado e invalido de proposito, para disparar
 * o caminho de erro que nao libera o semaforo. O segundo pedido e
 * valido, mas como o semaforo ja ficou "preso" no primeiro, o programa
 * trava em sem_wait(&mutex). Ha um watchdog de 5s para avisar. Este
 * bug e 100% reproduzivel (nem precisa de threads).
 */
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <semaphore.h>

typedef struct { int quantidade; } Pedido;

sem_t mutex;
int estoque = 100;

int processar_pedido(Pedido *p) {
    sem_wait(&mutex);

    if (p->quantidade <= 0) {
        printf("Pedido invalido (quantidade=%d)\n", p->quantidade);
        return -1;              /* <-- linha do bug: falta sem_post antes deste return */
    }

    estoque -= p->quantidade;
    printf("Pedido processado, estoque agora = %d\n", estoque);

    sem_post(&mutex);
    return 0;
}

void watchdog(int sig) {
    (void) sig;
    fprintf(stderr, "\n*** POSSIVEL DEADLOCK: mutex ficou preso e o segundo pedido nunca foi processado ***\n");
    exit(1);
}

int main(void) {
    sem_init(&mutex, 0, 1);
    signal(SIGALRM, watchdog);
    alarm(5);

    Pedido invalido = { .quantidade = -5 };
    Pedido valido   = { .quantidade = 10 };

    processar_pedido(&invalido);
    processar_pedido(&valido);     /* nunca deve chegar a imprimir seu resultado */

    printf("Programa terminou normalmente.\n");
    sem_destroy(&mutex);
    return 0;
}
