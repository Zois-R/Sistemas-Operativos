#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "procesos.h"
#include "semaforos.h"

void calcularPromedioSueldos(tEmpleado *empleados, int cant, int *terminar, int semid) {
    while (!*terminar) {
        waitSemaforo(semid);
        float suma = 0;
        for (int i = 0; i < cant; i++) {
            suma += empleados[i].sueldo;
        }
        printf("[Hijo 1] Promedio sueldos: %.2f\n", suma / cant);
        signalSemaforo(semid);
        sleep(2);
    }
}

void aumentarSueldosPorCategoria(tEmpleado *empleados, int cant, int *terminar, int semid) {
    while (!*terminar) {
        waitSemaforo(semid);
        for (int i = 0; i < cant; i++) {
            if (empleados[i].categoria == 1) {
                empleados[i].sueldo *= 1.1;
            }
        }
        printf("[Hijo 2] Aumentados sueldos cat.1\n");
        signalSemaforo(semid);
        sleep(2);
    }
}

void cambiarCategoriaPorAntiguedad(tEmpleado *empleados, int cant, int *terminar, int semid) {
    while (!*terminar) {
        waitSemaforo(semid);
        for (int i = 0; i < cant; i++) {
            if (empleados[i].antiguedad > 5  && empleados[i].antiguedad <= 10 && empleados[i].categoria == 'C')
            {
                empleados[i].categoria = 'B';
            }
            if (empleados[i].antiguedad > 10 && empleados[i].categoria == 'B')
            {
                empleados[i].categoria = 'A';
            }
            
        }


        printf("[Hijo 3] Cambiadas categorías por antigüedad\n");
        signalSemaforo(semid);
        sleep(2);
    }
}

void borrarEmpleadosConSueldoBajo(tEmpleado *empleados, int *cant, int *terminar, int semid) {
    while (!*terminar) {
        waitSemaforo(semid);
        int j = 0;
        for (int i = 0; i < *cant; i++) {
            if (empleados[i].sueldo >= 45000) {
                empleados[j++] = empleados[i];
            }
        }
        *cant = j;
        printf("[Hijo 4] Borrados empleados con sueldo bajo\n");
        signalSemaforo(semid);
        sleep(2);
    }
}



/**void mostrarEMpleadosFinal(tEmpleado *empleados,int *cant)
{
    while(*cant > 0)
    {
         printf("%-7d %-20s %-3c %10d %14.2f\n",empleados->legajo,empleados->nombre,
            empleados->categoria,empleados->antiguedad,empleados->sueldo);
        empleados++;
        cant--;
    }
}
*/
void mostrarEMpleadosFinal(tEmpleado *empleados, int *cant)
{
    for (int i = 0; i < *cant; i++)
    {
        printf("%-7d %-20s %-3c %10d %14.2f\n", empleados[i].legajo, empleados[i].nombre,
               empleados[i].categoria, empleados[i].antiguedad, empleados[i].sueldo);
    }
}