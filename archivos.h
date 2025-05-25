#ifndef ARCHIVOS_H_INCLUDED
#define ARCHIVOS_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tipos.h"
#define ERR_ARCH -1
#define TODO_OK 1



int crearArchivo(FILE **pf, const char *nombreArchivo);
void mostrarArchivo(FILE **pf,const char *nombreArchivo);
int aperturaArchivo(FILE **pf, const char *nombreArchivo, const char *apertura);
void cargarMemoriaDesdeArchivo(FILE *pf, tEmpleado *empleados, int *cant);



#endif // ARCHIVOS_H_INCLUDED
