/*
 * Trecho10WaitForaSync.java
 * Bug: lock.wait() chamado fora de um bloco synchronized sobre o
 * mesmo objeto.
 *
 * Compilar: javac Trecho10WaitForaSync.java
 * Rodar:    java Trecho10WaitForaSync
 *
 * O bug e 100% reproduzivel: a JVM lanca IllegalMonitorStateException
 * assim que aguardarItem() tenta chamar wait() sem ser dona do monitor.
 */
public class Trecho10WaitForaSync {

    static class Fila {
        private final Object lock = new Object();
        private int contador = 0;

        void aguardarItem() throws InterruptedException {
            while (contador == 0) {
                lock.wait();      // <-- linha do bug: falta 'synchronized (lock)' em volta
            }
            contador--;
        }

        void adicionarItem() {
            synchronized (lock) {
                contador++;
                lock.notifyAll();
            }
        }
    }

    public static void main(String[] args) throws InterruptedException {
        Fila fila = new Fila();
        fila.aguardarItem();
    }
}
