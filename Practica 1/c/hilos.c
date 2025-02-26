#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define TAMANO_BUFFER 5  // Tamaño del búfer compartido
#define NUM_PRODUCTORES 2  // Número de hilos productores
#define NUM_CONSUMIDORES 2  // Número de hilos consumidores

int buffer[TAMANO_BUFFER];  // Búfer compartido
int contador = 0;  // Contador de elementos en el búfer

// Mutex y variables de condición para sincronización
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t lleno = PTHREAD_COND_INITIALIZER;
pthread_cond_t vacio = PTHREAD_COND_INITIALIZER;

// Función del productor
void* productor(void* arg) {
    int id = *(int*)arg;
    free(arg);

    for (int i = 0; i < 10; i++) {  // Cada productor produce 10 elementos
        pthread_mutex_lock(&mutex);

        // Esperar si el búfer está lleno
        while (contador == TAMANO_BUFFER) {
            pthread_cond_wait(&vacio, &mutex);
        }

        // Producir un elemento y colocarlo en el búfer
        int item = rand() % 100;  // Generar un número aleatorio
        buffer[contador] = item;
        contador++;

        printf("Productor %d produjo: %d (Elementos en búfer: %d)\n", id, item, contador);

        // Señalar que el búfer ya no está vacío
        pthread_cond_signal(&lleno);
        pthread_mutex_unlock(&mutex);

        sleep(1);  // Simular un retardo en la producción
    }

    pthread_exit(NULL);
}

// Función del consumidor
void* consumidor(void* arg) {
    int id = *(int*)arg;
    free(arg);

    for (int i = 0; i < 10; i++) {  // Cada consumidor consume 10 elementos
        pthread_mutex_lock(&mutex);

        // Esperar si el búfer está vacío
        while (contador == 0) {
            pthread_cond_wait(&lleno, &mutex);
        }

        // Consumir un elemento del búfer
        int item = buffer[contador - 1];
        contador--;

        printf("Consumidor %d consumió: %d (Elementos en búfer: %d)\n", id, item, contador);

        // Señalar que el búfer ya no está lleno
        pthread_cond_signal(&vacio);
        pthread_mutex_unlock(&mutex);

        sleep(1);  // Simular un retardo en el consumo
    }

    pthread_exit(NULL);
}

int main() {
    pthread_t hilos_productores[NUM_PRODUCTORES];
    pthread_t hilos_consumidores[NUM_CONSUMIDORES];

    // Crear hilos productores
    for (int i = 0; i < NUM_PRODUCTORES; i++) {
        int* id = (int*)malloc(sizeof(int));
        *id = i + 1;
        if (pthread_create(&hilos_productores[i], NULL, productor, (void*)id) != 0) {
            perror("Error al crear el hilo productor");
            return 1;
        }
    }

    // Crear hilos consumidores
    for (int i = 0; i < NUM_CONSUMIDORES; i++) {
        int* id = (int*)malloc(sizeof(int));
        *id = i + 1;
        if (pthread_create(&hilos_consumidores[i], NULL, consumidor, (void*)id) != 0) {
            perror("Error al crear el hilo consumidor");
            return 1;
        }
    }

    // Esperar a que los hilos productores terminen
    for (int i = 0; i < NUM_PRODUCTORES; i++) {
        pthread_join(hilos_productores[i], NULL);
    }

    // Esperar a que los hilos consumidores terminen
    for (int i = 0; i < NUM_CONSUMIDORES; i++) {
        pthread_join(hilos_consumidores[i], NULL);
    }

    // Destruir el mutex y las variables de condición
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&lleno);
    pthread_cond_destroy(&vacio);

    printf("Todos los hilos han terminado.\n");

    return 0;
}
