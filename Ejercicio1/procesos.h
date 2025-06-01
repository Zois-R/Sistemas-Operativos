#ifndef PROCESOS_H
#define PROCESOS_H

#include <stdio.h>
#include <stdlib.h>
#include "tipos.h"

void calcularPromedioSueldos(tEmpleado *empleados, int *cant, int *terminar, int semid);
void aumentarSueldosPorCategoria(tEmpleado *empleados, int *cant, int *terminar, int semid) ;
void cambiarCategoriaPorAntiguedad(tEmpleado *empleados, int *cant, int *terminar, int semid); 
void borrarEmpleadosConSueldoBajo(tEmpleado *empleados, int *cant, int *terminar, int semid);
void mostrarEMpleadosFinal(tEmpleado *empleados,int *cant);
#endif