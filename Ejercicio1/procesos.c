#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include "procesos.h"
#include "semaforos.h"

float calcularPromedioSueldosAux(tEmpleado *empleados, int *cant);

float calcularPromedioSueldosAux(tEmpleado *empleados, int *cant) {
    float suma = 0;
    for (int i = 0; i < *cant; i++) {
        suma += empleados[i].sueldo;
    }
    return suma / *cant;
}

void calcularPromedioSueldos(tEmpleado *empleados, int *cant, int *terminar, int semid) {
    while (!*terminar) {
        waitSemaforo(semid);
        float promedio = calcularPromedioSueldosAux(empleados, cant);
        signalSemaforo(semid);
        printf("[Hijo 1] Promedio sueldos: %.2f\n", promedio);
        sleep(2);
    }
}

void aumentarSueldosPorCategoria(tEmpleado *empleados, int *cant, int *terminar, int semid) {
    int idx;
    char categoria;
    char letras[] = {'A', 'B', 'C'};
    srand(time(NULL)); // Inicializamos la semilla para generar números aleatorios
    while (!*terminar) {
        idx = rand() % 3;
        categoria = letras[idx];
        waitSemaforo(semid);
        for (int i = 0; i < *cant; i++) {
            if (empleados[i].categoria == categoria) {
                empleados[i].sueldo *= 1.05;
            }
        }
        signalSemaforo(semid);
        printf("[Hijo 2] Aumentados sueldos cat %c\n", categoria);
        sleep(2);
    }
}

void cambiarCategoriaPorAntiguedad(tEmpleado *empleados, int *cant, int *terminar, int semid) {
    while (!*terminar) {
        waitSemaforo(semid);
        for (int i = 0; i < *cant; i++) {
            if (empleados[i].antiguedad > 5  && empleados[i].antiguedad <= 10 && empleados[i].categoria == 'C')
            {
                empleados[i].categoria = 'B';
            }
            if (empleados[i].antiguedad > 10 && empleados[i].categoria == 'B')
            {
                empleados[i].categoria = 'A';
            }
            
        }
        signalSemaforo(semid);
        printf("[Hijo 3] Cambiadas categorías por antigüedad\n");
        sleep(2);
    }
}

void borrarEmpleadosConSueldoBajo(tEmpleado *empleados, int *cant, int *terminar, int semid) {
    while (!*terminar) {
        waitSemaforo(semid);
        int j = 0, cant2 = 0;
        float umbralSueldo = calcularPromedioSueldosAux(empleados, cant) - 10000;
        for (int i = 0; i < *cant; i++) {
            if (empleados[i].sueldo >= umbralSueldo) { // si el sueldo esta por debajo del umbralSueldo se borra el empleado
                empleados[j] = empleados[i];
                j++;
                cant2++;
            }
        }
        *cant = cant2;
        signalSemaforo(semid);
        printf("[Hijo 4] Borrados empleados con sueldo bajo (menor a %.2f)\n", umbralSueldo);
        sleep(2);
    }
}

void mostrarEMpleadosFinal(tEmpleado *empleados, int *cant)
{
    for (int i = 0; i < *cant; i++)
    {
        printf("%-7d %-20s %-3c %10d %14.2f\n", empleados[i].legajo, empleados[i].nombre,
               empleados[i].categoria, empleados[i].antiguedad, empleados[i].sueldo);
    }
}