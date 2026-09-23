/*
 * Trecho11Correto.java
 * CONTROLE: buffer com monitor corretamente implementado (while +
 * wait + notifyAll), igual a solucao do laboratorio principal.
 *
 * Compilar: javac Trecho11Correto.java
 * Rodar:    java Trecho11Correto
 *
 * Rode varias vezes, inclusive aumentando N_PRODUTORES/N_CONSUMIDORES
 * como no Trecho 8, para confirmar que, com 'while', o problema nao
 * aparece mesmo sob a mesma carga que quebrou o Trecho 8. Nao ha bug
 * aqui para corrigir.
 */
import java.util.concurrent.atomic.AtomicInteger;

public class Trecho11Correto {

    static final int TAM_BUFFER = 3;
    static final int N_PRODUTORES = 4;
    static final int N_CONSUMIDORES = 4;
    static final int ITENS_POR_PRODUTOR = 500;

    static class Buffer {
        int[] dados = new int[TAM_BUFFER];
        int inicio = 0, fim = 0, contador = 0;

        synchronized void inserir(int item) throws InterruptedException {
            while (contador == TAM_BUFFER) {
                wait();
            }
            dados[fim] = item;
            fim = (fim + 1) % TAM_BUFFER;
            contador++;
            notifyAll();
        }

        synchronized int remover() throws InterruptedException {
            while (contador == 0) {
                wait();
            }
            int item = dados[inicio];
            inicio = (inicio + 1) % TAM_BUFFER;
            contador--;
            notifyAll();
            return item;
        }
    }

    public static void main(String[] args) throws InterruptedException {
        Buffer buffer = new Buffer();
        AtomicInteger proximoId = new AtomicInteger(0);
        Thread[] produtores = new Thread[N_PRODUTORES];
        Thread[] consumidores = new Thread[N_CONSUMIDORES];

        for (int i = 0; i < N_PRODUTORES; i++) {
            produtores[i] = new Thread(() -> {
                for (int j = 0; j < ITENS_POR_PRODUTOR; j++) {
                    try {
                        buffer.inserir(proximoId.getAndIncrement());
                    } catch (InterruptedException e) {
                        Thread.currentThread().interrupt();
                    }
                }
            });
            produtores[i].start();
        }
        int totalItens = N_PRODUTORES * ITENS_POR_PRODUTOR;
        for (int i = 0; i < N_CONSUMIDORES; i++) {
            consumidores[i] = new Thread(() -> {
                for (int j = 0; j < totalItens / N_CONSUMIDORES; j++) {
                    try {
                        buffer.remover();
                    } catch (InterruptedException e) {
                        Thread.currentThread().interrupt();
                    }
                }
            });
            consumidores[i].start();
        }

        for (Thread t : produtores) t.join();
        for (Thread t : consumidores) t.join();
        System.out.println("Execucao terminou sem nenhum erro (esperado, ja que o codigo esta correto).");
    }
}
