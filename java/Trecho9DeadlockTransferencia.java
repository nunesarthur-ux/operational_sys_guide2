/*
 * Trecho9DeadlockTransferencia.java
 * Bug: a ordem de aquisicao dos monitores depende da ordem dos
 * parametros, que pode se inverter entre chamadas concorrentes.
 *
 * Compilar: javac Trecho9DeadlockTransferencia.java
 * Rodar:    java Trecho9DeadlockTransferencia
 *
 * Ha um atraso deliberado (Thread.sleep) entre os dois synchronized
 * aninhados, so para tornar o deadlock facil de observar a cada
 * execucao. Um watchdog de 5s avisa se o programa travou.
 */
public class Trecho9DeadlockTransferencia {

    static class Conta {
        int saldo;
        Conta(int saldo) { this.saldo = saldo; }
    }

    static void transferir(Conta origem, Conta destino, int valor) {
        synchronized (origem) {
            System.out.println(Thread.currentThread().getName()
                    + " pegou o lock de origem, tentando pegar o de destino...");
            try { Thread.sleep(100); } catch (InterruptedException ignored) {}
            synchronized (destino) {
                origem.saldo -= valor;
                destino.saldo += valor;
                System.out.println(Thread.currentThread().getName() + " completou a transferencia");
            }
        }
    }

    public static void main(String[] args) throws InterruptedException {
        Conta contaX = new Conta(1000);
        Conta contaY = new Conta(1000);

        Thread watchdog = new Thread(() -> {
            try {
                Thread.sleep(5000);
                System.out.println("\n*** POSSIVEL DEADLOCK: o programa nao terminou em 5s ***");
                Runtime.getRuntime().halt(1);
            } catch (InterruptedException ignored) {}
        });
        watchdog.setDaemon(true);
        watchdog.start();

        Thread t1 = new Thread(() -> transferir(contaX, contaY, 100), "thread-1 (X->Y)");
        Thread t2 = new Thread(() -> transferir(contaY, contaX, 50), "thread-2 (Y->X)");

        t1.start();
        t2.start();
        t1.join();
        t2.join();

        System.out.println("Programa terminou normalmente (sem deadlock nesta execucao).");
    }
}
