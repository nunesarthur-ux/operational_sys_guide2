/*
 * trecho5_produtor_consumidor.c
 * Bug: o produtor sinaliza o semaforo errado apos inserir um item
 * (sem_post(&vazio) de novo, em vez de sem_post(&cheio)) -- um erro
 * classico de "copiar e colar".
 *
 * Compilar: gcc -Wall -Wextra -pthread trecho5_produtor_consumidor.c -o trecho5
 * Rodar:    ./trecho5
 *
 * Como o consumidor nunca recebe sinal de 'cheio', ele fica bloqueado
 * para sempre no primeiro sem_wait(&cheio), mesmo com itens disponiveis
 * no buffer (lost wakeup). Ha um watchdog de 5s para avisar; e 100%
 * reproduzivel.
 */
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include <semaphore.h>

#define TAM_BUFFER  5
#define N_ITENS     10

int buffer_dados[TAM_BUFFER];
int fim = 0, inicio = 0;
sem_t mutex, vazio, cheio;

void *produtor(void *arg) {
    (void) arg;
    for (int i = 0; i < N_ITENS; i++) {
        sem_wait(&vazio);
        sem_wait(&mutex);

        buffer_dados[fim] = i;
        fim = (fim + 1) % TAM_BUFFER;
        printf("[produtor] inseriu item %d\n", i);

        sem_post(&mutex);
        sem_post(&vazio);   /* <-- linha do bug: deveria ser sem_post(&cheio) */
    }
    return NULL;
}

void *consumidor(void *arg) {
    (void) arg;
    for (int i = 0; i < N_ITENS; i++) {
        sem_wait(&cheio);
        sem_wait(&mutex);

        int item = buffer_dados[inicio];
        inicio = (inicio + 1) % TAM_BUFFER;
        printf("[consumidor] retirou item %d\n", item);

        sem_post(&mutex);
        sem_post(&vazio);
    }
    return NULL;
}

void watchdog(int sig) {
    (void) sig;
    fprintf(stderr, "\n*** POSSIVEL DEADLOCK / LOST WAKEUP: consumidor nunca recebeu sinal de 'cheio' ***\n");
    exit(1);
}

int main(void) {
    sem_init(&mutex, 0, 1);
    sem_init(&vazio, 0, TAM_BUFFER);
    sem_init(&cheio, 0, 0);
    signal(SIGALRM, watchdog);
    alarm(5);

    pthread_t p, c;
    pthread_create(&p, NULL, produtor, NULL);
    pthread_create(&c, NULL, consumidor, NULL);

    pthread_join(p, NULL);
    pthread_join(c, NULL);

    printf("Programa terminou normalmente.\n");
    return 0;
}
