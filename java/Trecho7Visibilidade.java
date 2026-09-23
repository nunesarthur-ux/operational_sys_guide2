/*
 * Trecho7Visibilidade.java
 * Bug: incrementar() e synchronized, mas getValor() nao e -- uma
 * thread lendo o valor por getValor() nao tem garantia de enxergar a
 * escrita mais recente feita por outra thread (problema de
 * VISIBILIDADE, diferente do problema de atomicidade do Trecho 6).
 *
 * Compilar: javac Trecho7Visibilidade.java
 * Rodar:    java Trecho7Visibilidade
 *
 * AVISO IMPORTANTE: bugs de visibilidade sao os mais dificeis de
 * forcar de forma confiavel -- dependem de JIT, hardware, e de o que
 * mais o programa faz (chamadas de I/O como System.out.println() costumam
 * criar barreiras de memoria "de brinde" que escondem o bug). Este
 * programa tenta expor o problema com uma thread leitora em busy-wait,
 * sem nenhuma chamada de I/O dentro do loop.
 *
 * Se, no seu computador, ele sempre terminar rapido e "sem bug", isso
 * NAO significa que o codigo esta correto -- significa que a
 * JVM/hardware que voce usa disfarça o problema. Isso tambem e um
 * resultado valido para discutir: codigo sem sincronizacao correta nao
 * tem NENHUMA garantia, mesmo que "funcione" na pratica.
 */
public class Trecho7Visibilidade {

    static final int ALVO = 1;
    static final long TIMEOUT_MS = 3000;

    private int valor = 0;

    public synchronized void incrementar() {
        valor++;
    }

    public int getValor() {
        return valor;   // <-- linha do bug: falta 'synchronized' (ou 'volatile' no campo)
    }

    public static void main(String[] args) throws InterruptedException {
        Trecho7Visibilidade contador = new Trecho7Visibilidade();
        final boolean[] viuAtualizacao = { false };

        Thread escritor = new Thread(() -> {
            try { Thread.sleep(200); } catch (InterruptedException ignored) {}
            contador.incrementar();
        });

        Thread leitor = new Thread(() -> {
            long inicio = System.currentTimeMillis();
            while (contador.getValor() < ALVO) {
                if (System.currentTimeMillis() - inicio > TIMEOUT_MS) {
                    return;   // desiste: nunca viu a atualizacao a tempo
                }
            }
            viuAtualizacao[0] = true;
        });

        leitor.start();
        escritor.start();
        escritor.join();
        leitor.join(TIMEOUT_MS + 1000);

        if (viuAtualizacao[0]) {
            System.out.println("A thread leitora viu a atualizacao (nesta execucao, neste hardware/JVM).");
        } else {
            System.out.println("*** BUG OBSERVADO: a thread leitora NUNCA viu a atualizacao dentro do timeout ***");
        }
    }
}
