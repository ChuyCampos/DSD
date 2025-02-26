public class ProductorConsumidor {
    private static final int TAMANO_BUFFER = 5;  // Tamaño del búfer compartido
    private static final int NUM_PRODUCTORES = 2;  // Número de hilos productores
    private static final int NUM_CONSUMIDORES = 2;  // Número de hilos consumidores

    private int[] buffer = new int[TAMANO_BUFFER];  // Búfer compartido
    private int contador = 0;  // Contador de elementos en el búfer

    // Objeto para sincronización
    private final Object lock = new Object();

    // Función del productor
    class Productor implements Runnable {
        private int id;

        public Productor(int id) {
            this.id = id;
        }

        @Override
        public void run() {
            for (int i = 0; i < 10; i++) {  // Cada productor produce 10 elementos
                synchronized (lock) {
                    // Esperar si el búfer está lleno
                    while (contador == TAMANO_BUFFER) {
                        try {
                            lock.wait();  // Esperar a que haya espacio en el búfer
                        } catch (InterruptedException e) {
                            Thread.currentThread().interrupt();
                        }
                    }

                    // Producir un elemento y colocarlo en el búfer
                    int item = (int) (Math.random() * 100);  // Generar un número aleatorio
                    buffer[contador] = item;
                    contador++;

                    System.out.printf("Productor %d produjo: %d (Elementos en búfer: %d)\n", id, item, contador);

                    // Notificar a los consumidores que hay un nuevo elemento
                    lock.notifyAll();
                }

                try {
                    Thread.sleep(1000);  // Simular un retardo en la producción
                } catch (InterruptedException e) {
                    Thread.currentThread().interrupt();
                }
            }
        }
    }

    // Función del consumidor
    class Consumidor implements Runnable {
        private int id;

        public Consumidor(int id) {
            this.id = id;
        }

        @Override
        public void run() {
            for (int i = 0; i < 10; i++) {  // Cada consumidor consume 10 elementos
                synchronized (lock) {
                    // Esperar si el búfer está vacío
                    while (contador == 0) {
                        try {
                            lock.wait();  // Esperar a que haya elementos en el búfer
                        } catch (InterruptedException e) {
                            Thread.currentThread().interrupt();
                        }
                    }

                    // Consumir un elemento del búfer
                    int item = buffer[contador - 1];
                    contador--;

                    System.out.printf("Consumidor %d consumió: %d (Elementos en búfer: %d)\n", id, item, contador);

                    // Notificar a los productores que hay espacio en el búfer
                    lock.notifyAll();
                }

                try {
                    Thread.sleep(1000);  // Simular un retardo en el consumo
                } catch (InterruptedException e) {
                    Thread.currentThread().interrupt();
                }
            }
        }
    }

    public static void main(String[] args) {
        ProductorConsumidor pc = new ProductorConsumidor();

        // Crear hilos productores
        for (int i = 0; i < NUM_PRODUCTORES; i++) {
            Thread productor = new Thread(pc.new Productor(i + 1));
            productor.start();
        }

        // Crear hilos consumidores
        for (int i = 0; i < NUM_CONSUMIDORES; i++) {
            Thread consumidor = new Thread(pc.new Consumidor(i + 1));
            consumidor.start();
        }
    }
}
