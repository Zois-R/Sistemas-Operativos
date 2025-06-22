#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/select.h>
#include <sys/prctl.h>
#include "archivos.h"
#include "memoria.h"
#include "procesos.h"
#include "semaforos.h"

#define N_HIJOS 4

int shmid = -1, semid = -1;
pid_t hijos[N_HIJOS];
tEmpleado *empleados = NULL;
int *terminar = NULL, *cantEmpleados = NULL;

volatile sig_atomic_t terminarPrograma = 0;

void handler(int sig) {
    //printf("\n[Padre] Recibí señal %d. Preparando salida...\n", sig);
    terminarPrograma = 1;
}

void finalizarHijos() {
    *terminar = 1;
    for (int i = 0; i < N_HIJOS; i++) {
        if (hijos[i] > 0) {
            kill(hijos[i], SIGTERM);
            waitpid(hijos[i], NULL, 0);
            printf("\tHijo %d terminado.\n", hijos[i]);
        }
    }
}

int main() {
    FILE *pf;

    // Señales
    struct sigaction sa = {0};
    sa.sa_handler = handler;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);

    // Archivo inicial
    aperturaArchivo(&pf, "Empleados.bin", "wb");
    crearArchivo(&pf, "Empleados.bin");
    fclose(pf);

    aperturaArchivo(&pf, "Empleados.bin", "rb");
    mostrarArchivo(&pf, "Empleados.bin");
    fclose(pf);

    // Memoria compartida
    shmid = crearMemoriaCompartida(sizeof(tEmpleado) * CANT_EMPLEADOS + sizeof(int) + sizeof(int));
    if (shmid == -1) exit(EXIT_FAILURE);

    empleados = (tEmpleado*)asociarMemoria(shmid);
    if (!empleados) exit(EXIT_FAILURE);

    terminar = (int*)(empleados + CANT_EMPLEADOS);
    *terminar = 0;
    cantEmpleados = (int*)(terminar + 1);

    aperturaArchivo(&pf, "Empleados.bin", "rb");
    cargarMemoriaDesdeArchivo(pf, empleados, cantEmpleados);
    fclose(pf);

    // Semáforo
    semid = crearSemaforo();
    inicializarSemaforo(semid, 1);

    printf("\nPresione ENTER para comenzar...\n");
    getchar();

    printf("Presione 's' + ENTER para finalizar o Ctrl+C.\n");

    // Crear hijos
    for (int i = 0; i < N_HIJOS; i++) {
        hijos[i] = fork();
        if (hijos[i] < 0) {
            perror("Error en fork");
            terminarPrograma = 1;
            break;
        } else if (hijos[i] == 0) {
            prctl(PR_SET_PDEATHSIG, SIGTERM);
            if (getppid() == 1) exit(EXIT_FAILURE);
            signal(SIGTERM, SIG_DFL);
            switch(i) {
                case 0: calcularPromedioSueldos(empleados, cantEmpleados, terminar, semid); break;
                case 1: aumentarSueldosPorCategoria(empleados, cantEmpleados, terminar, semid); break;
                case 2: cambiarCategoriaPorAntiguedad(empleados, cantEmpleados, terminar, semid); break;
                case 3: borrarEmpleadosConSueldoBajo(empleados, cantEmpleados, terminar, semid); break;
            }
            exit(0);
        }
    }

    // Loop principal con select + supervisión de hijos
    while (!terminarPrograma) {
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds);
        struct timeval tv = {1, 0};

        int r = select(STDIN_FILENO + 1, &readfds, NULL, NULL, &tv);
        if (r > 0 && FD_ISSET(STDIN_FILENO, &readfds)) {
            char c = getchar();
            if (c == 's') {
                terminarPrograma = 1;
            } else {
                printf("Presione 's' para terminar o Ctrl+C.\n");
            }
        }

        // Supervisar hijos
        for (int i = 0; i < N_HIJOS; i++) {
            if (hijos[i] > 0) {
                int status;
                pid_t result = waitpid(hijos[i], &status, WNOHANG);
                if (result == hijos[i]) {
                    printf("[Padre] Detecté que el hijo %d terminó inesperadamente.\n", hijos[i]);
                    terminarPrograma = 1;
                }
            }
        }
    }

    finalizarHijos();

    printf("\nListado final de empleados:\n");
    mostrarEMpleadosFinal(empleados, cantEmpleados);

    // Limpieza
    if (empleados) liberarMemoria(empleados);
    if (shmid != -1) eliminarMemoria(shmid);
    if (semid != -1) eliminarSemaforo(semid);

    printf("Recursos liberados correctamente.\n");
    return 0;
}
