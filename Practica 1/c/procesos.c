#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#define NUM_HIJOS 3  // Número de procesos hijos a crear

// Estructura para enviar información al padre
typedef struct {
    int id_hijo;
    float uso_cpu;
    float uso_memoria;
    char info_red[256];
} InfoSistema;

void obtener_info_red(char *buffer) {
    // Ejecutar el comando "ifconfig" o "ip" para obtener información de red
    FILE *fp = popen("ip addr", "r");  // Usar "ip addr" en sistemas modernos
    if (fp == NULL) {
        perror("Error al ejecutar ifconfig");
        strcpy(buffer, "No disponible");
        return;
    }

    // Leer la salida del comando
    char linea[256];
    while (fgets(linea, sizeof(linea), fp) != NULL) {
        strcat(buffer, linea);
    }

    pclose(fp);
}

int main() {
    int tuberia[2];  // Descriptores de la tubería
    pid_t pids[NUM_HIJOS];  // Almacenar los PIDs de los hijos
    int i;

    // Crear la tubería
    if (pipe(tuberia) == -1) {
        perror("Error al crear la tubería");
        exit(EXIT_FAILURE);
    }

    // Crear múltiples procesos hijos
    for (i = 0; i < NUM_HIJOS; i++) {
        pids[i] = fork();

        if (pids[i] < 0) {
            perror("Error al crear el proceso hijo");
            exit(EXIT_FAILURE);
        } else if (pids[i] == 0) {
            // Código ejecutado por el proceso hijo
            close(tuberia[0]);  // Cerrar el extremo de lectura de la tubería

            InfoSistema info;
            info.id_hijo = i;

            // Simular el uso de CPU y memoria
            info.uso_cpu = (float)(rand() % 100);  // Uso de CPU aleatorio (0-100%)
            info.uso_memoria = (float)(rand() % 100);  // Uso de memoria aleatorio (0-100%)

            // Obtener información de red
            obtener_info_red(info.info_red);

            // Enviar la información al padre a través de la tubería
            write(tuberia[1], &info, sizeof(info));

            close(tuberia[1]);  // Cerrar el extremo de escritura de la tubería
            exit(0);  // Terminar el proceso hijo
        }
    }

    // Código ejecutado por el proceso padre
    close(tuberia[1]);  // Cerrar el extremo de escritura de la tubería

    InfoSistema info_hijo;
    printf("Padre: Recibiendo información de los hijos...\n");

    // Leer la información de los hijos desde la tubería
    for (i = 0; i < NUM_HIJOS; i++) {
        read(tuberia[0], &info_hijo, sizeof(info_hijo));
        printf("\nInformación del hijo %d:\n", info_hijo.id_hijo);
        printf("  Uso de CPU: %.2f%%\n", info_hijo.uso_cpu);
        printf("  Uso de memoria: %.2f%%\n", info_hijo.uso_memoria);
        printf("  Información de red:\n%s\n", info_hijo.info_red);
    }

    close(tuberia[0]);  // Cerrar el extremo de lectura de la tubería

    // Esperar a que todos los hijos terminen
    for (i = 0; i < NUM_HIJOS; i++) {
        wait(NULL);
    }

    printf("Padre: Todos los hijos han terminado.\n");

    return 0;
}
