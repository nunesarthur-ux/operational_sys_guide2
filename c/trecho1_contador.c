/*
 * trecho1_contador.c
 * Bug: condicao de corrida por falta de exclusao mutua (atomicidade).
 *
 * Compilar: gcc -Wall -Wextra -pthread trecho1_contador.c -o trecho1
 * Rodar:    ./trecho1
 *
 * Para forcar o bug a aparecer de forma confiavel, aumente N_THREADS
 * e/ou N_ITERACOES abaixo. Com poucas iteracoes o SO pode nao trocar
 * de thread no meio do "contador++", e o resultado sai certo por
 * sorte -- rode varias vezes se nao ver o erro de primeira.
 */
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <stdbool.h>


#define N_THREADS    4
#define N_ITERACOES  200000

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int contador = 0;

void *tarefa(void *arg) {
    (void) arg;
    for (int i = 0; i < N_ITERACOES; i++) {
        pthread_mutex_lock(&mutex);
        contador++;   /* <-- linha do bug: leitura+incremento+escrita sem protecao */
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

int main(void) {
    pthread_t threads[N_THREADS];

    for (int i = 0; i < N_THREADS; i++) {
        pthread_create(&threads[i], NULL, tarefa, NULL);
    }
    for (int i = 0; i < N_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    int esperado = N_THREADS * N_ITERACOES;
    printf("Esperado: %d\n", esperado);
    printf("Obtido:   %d\n", contador);
    if (contador != esperado) {
        printf("*** BUG CONFIRMADO: %d incrementos foram perdidos ***\n", esperado - contador);
    } else {
        printf("Nenhuma perda detectada nesta execucao (rode de novo - o bug e probabilistico).\n");
    }
    return 0;
}
