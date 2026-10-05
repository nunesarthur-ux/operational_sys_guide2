CC=gcc
CFLAGS=-Wall -Wextra -pthread

TRECHOS=trecho1 trecho2 trecho3 trecho4 trecho5

all: $(TRECHOS)

trecho1: trecho1_contador.c
	$(CC) $(CFLAGS) trecho1_contador.c -o trecho1

trecho2: trecho2_deadlock_locks.c
	$(CC) $(CFLAGS) trecho2_deadlock_locks.c -o trecho2

trecho3: trecho3_mutex_vazado.c
	$(CC) $(CFLAGS) trecho3_mutex_vazado.c -o trecho3

trecho4: trecho4_semaforo_init.c
	$(CC) $(CFLAGS) trecho4_semaforo_init.c -o trecho4

trecho5: trecho5_produtor_consumidor.c
	$(CC) $(CFLAGS) trecho5_produtor_consumidor.c -o trecho5

clean:
	rm -f $(TRECHOS)

.PHONY: all clean
