/*
 * trecho4_semaforo_init.c
 * Bug: semaforo usado como mutex, mas inicializado com valor 0 em vez
 * de 1 (inicializacao incorreta).
 *
 * Compilar: gcc -Wall -Wextra -pthread trecho4_semaforo_init.c -o trecho4
 * Rodar:    ./trecho4
 *
 * O bug e 100% reproduzivel: a primeira chamada a sem_wait(&mutex) ja
 * trava, porque nenhuma thread jamais chamou sem_post antes. Ha um
 * watchdog de 3s para avisar (nem precisa de threads para observar).
 */
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <semaphore.h>

sem_t mutex;

void watchdog(int sig) {
    (void) sig;
    fprintf(stderr, "\n*** POSSIVEL DEADLOCK: sem_wait(&mutex) nunca retornou ***\n");
    exit(1);
}

int main(void) {
    sem_init(&mutex, 0, 0);   /* <-- linha do bug: deveria ser 1 */
    signal(SIGALRM, watchdog);
    alarm(3);

    printf("Antes de sem_wait...\n");
    sem_wait(&mutex);
    printf("Depois de sem_wait (isto nunca deveria ser impresso).\n");

    sem_post(&mutex);
    sem_destroy(&mutex);
    return 0;
}
