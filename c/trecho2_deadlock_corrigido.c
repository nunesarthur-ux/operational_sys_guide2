/*
 * trecho2_deadlock_locks.c
 * Bug: deadlock por ordem de aquisicao de locks invertida entre threads.
 *
 * Compilar: gcc -Wall -Wextra -pthread trecho2_deadlock_locks.c -o trecho2
 * Rodar:    ./trecho2
 *
 * O programa tem um alarme de 5s: se ele nao terminar sozinho ate la,
 * um watchdog imprime "POSSIVEL DEADLOCK" e encerra o processo, para
 * voce nao ficar com o terminal travado sem saber o que houve.
 *
 * O usleep() entre os dois locks e um atraso deliberado, so para
 * tornar o deadlock facil de observar a cada execucao (o bug existe
 * com ou sem ele, mas sem ele pode nao aparecer sempre).
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>

pthread_mutex_t A = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t B = PTHREAD_MUTEX_INITIALIZER;

void *thread1(void *arg) {
    (void) arg;
    pthread_mutex_lock(&A);
    printf("[thread1] pegou A, tentando pegar B...\n");
    usleep(100000);
    pthread_mutex_unlock(&A);
    pthread_mutex_lock(&B);

    printf("[thread1] pegou A e B\n");

    pthread_mutex_unlock(&B);
    return NULL;
}

void *thread2(void *arg) {
    (void) arg;
    pthread_mutex_lock(&B);
    printf("[thread2] pegou B, tentando pegar A...\n");
    usleep(100000);
    pthread_mutex_unlock(&B);
    pthread_mutex_lock(&A);

    printf("[thread2] pegou B e A\n");

    pthread_mutex_unlock(&A);
    return NULL;
}

void watchdog(int sig) {
    (void) sig;
    fprintf(stderr, "\n*** POSSIVEL DEADLOCK: o programa nao terminou em 5s ***\n");
    exit(1);
}

int main(void) {
    signal(SIGALRM, watchdog);
    alarm(5);

    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread1, NULL);
    pthread_create(&t2, NULL, thread2, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    printf("Programa terminou normalmente (sem deadlock nesta execucao).\n");
    return 0;
}
