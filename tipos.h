#ifndef TIPOS_H
#define TIPOS_H

#define CANT_EMPLEADOS 20
typedef struct {
    int legajo;
    char nombre[50];
    char categoria;
    int antiguedad;
    float sueldo;
} tEmpleado;

#endif