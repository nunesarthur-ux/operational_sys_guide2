/*
 * Trecho6Contador.java
 * Bug: condicao de corrida por falta de sincronizacao (atomicidade).
 *
 * Compilar: javac Trecho6Contador.java
 * Rodar:    java Trecho6Contador
 *
 * Para forcar o bug a aparecer de forma confiavel, aumente N_THREADS
 * e/ou N_ITERACOES abaixo. Com poucas iteracoes o resultado pode sair
 * certo por sorte -- rode varias vezes se nao ver o erro de primeira.
 */
public class Trecho6Contador {

    static final int N_THREADS = 4;
    static final int N_ITERACOES = 200_000;

    static int valor = 0;

    static void incrementar() {
        valor++;   // <-- linha do bug: leitura+incremento+escrita sem protecao
    }

    public static void main(String[] args) throws InterruptedException {
        Thread[] threads = new Thread[N_THREADS];
        for (int i = 0; i < N_THREADS; i++) {
            threads[i] = new Thread(() -> {
                for (int j = 0; j < N_ITERACOES; j++) {
                    incrementar();
                }
            });
            threads[i].start();
        }
        for (Thread t : threads) t.join();

        int esperado = N_THREADS * N_ITERACOES;
        System.out.println("Esperado: " + esperado);
        System.out.println("Obtido:   " + valor);
        if (valor != esperado) {
            System.out.println("*** BUG CONFIRMADO: " + (esperado - valor) + " incrementos foram perdidos ***");
        } else {
            System.out.println("Nenhuma perda detectada nesta execucao (rode de novo - o bug e probabilistico).");
        }
    }
}
