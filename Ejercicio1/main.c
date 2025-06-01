#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include "archivos.h"
#include "memoria.h"
#include "procesos.h"
#include "semaforos.h"
int main()
{
    int shmid; //Nos va a dar el id del recurso de la memoria
    FILE *pf;
    char c;
    tEmpleado *empleados;
    int semid;
    int cantEmpleados;
    char nombreArchivo[] = "Empleados.bin";
    __pid_t pid1,pid2,pid3,pid4;
    //Esto corresponde a la creación de la estructura del archivo
    aperturaArchivo(&pf,nombreArchivo,"wb");
    crearArchivo(&pf,nombreArchivo);
    fclose(pf);


    aperturaArchivo(&pf,nombreArchivo,"rb");
    mostrarArchivo(&pf,nombreArchivo);
    fclose(pf);

    ///Acá todo lo relacionado con la memoria compartida

    shmid = crearMemoriaCompartida(sizeof(tEmpleado) * CANT_EMPLEADOS +sizeof(int));
    if( shmid == -1)
    {
        perror("Error al crear memoria compartida\n");
        return 1;
    }
    
    //Ahora tenemos que asociar la memoria al espacio de direcciones
    ///Para poder utilizar el puntero del vector

    empleados = (tEmpleado*)asociarMemoria(shmid);
     if(!empleados)
     {
        perror("Error al asociar memoria\n");
        return 1;
     }
    int *terminar = (int*)(empleados + CANT_EMPLEADOS);
    *terminar = 0;
   


    //CARGAMOS LOS REGISTROS DEL ARCHIVO EN LA MEMORIA COMPARTIDA CREADA

    aperturaArchivo(&pf,nombreArchivo,"rb");
    cargarMemoriaDesdeArchivo(pf,empleados,&cantEmpleados);
    fclose(pf);

     //Creamos el semáforo para sincronizar el acceso a empleados.
     semid = crearSemaforo(1);
     if(semid == -1)
     {
        perror("Error al crear semáforo \n");
        return 1;
     }

     inicializarSemaforo(semid,1);

    printf("\nA continuación comienzan las operaciones de los procesos hijos. Luego de continuar inserte 's' para finalizar los procesos hijos.\n");
    printf("\nPresione la tecla enter para continuar...\n");
    getchar();

    pid1 = fork();
    if (pid1 < 0) {
        perror("Error al hacer fork");
        return 1;
    } else if (pid1 == 0) {
        calcularPromedioSueldos(empleados, cantEmpleados, terminar,semid);
        exit(0);
    }


    pid2 = fork();
    if (pid2 < 0) {
        perror("Error al hacer fork");
        return 1;
    } else if (pid2 == 0) {
        aumentarSueldosPorCategoria(empleados,cantEmpleados, terminar,semid);
        exit(0);
    }


    pid3 = fork();
    if (pid3 < 0) {
        perror("Error al hacer fork");
        return 1;
    } else if (pid3 == 0) 
    {
        cambiarCategoriaPorAntiguedad(empleados,cantEmpleados,terminar,semid);
        exit(0);
    }


    pid4 = fork();
    if (pid4 < 0) 
    {
        perror("Error al hacer fork");
        return 1;
    } else if (pid4 == 0)
     {
        borrarEmpleadosConSueldoBajo(empleados, &cantEmpleados,terminar,semid);
        exit(0);
    }

    c = getchar();
    while (getchar() != '\n' && getchar() != EOF); // Limpiar el buffer de entrada
    while(c != 's')
    {
        printf("\n\nDebe presionar 's' para finalizar los procesos hijos...\n\n");
        c = getchar();
        while (getchar() != '\n' && getchar() != EOF); // Limpiar el buffer de entrada
    }

    *terminar = 1; //Ya finalizó

    //Con esto esperamos a que todos los hijos terminen
    for (int i = 0; i < 4; i++) 
    {
        wait(NULL);
    }

    printf("\nListado de empleados luego de que los procesos hayan realizados sus operaciones\n\n");

    mostrarEMpleadosFinal(empleados,&cantEmpleados);
    ///Ahora tenemos que liberar la memoria compartida ya que los procesos terminaron

    if(liberarMemoria(empleados) == -1)
    {
        fprintf(stderr, "No se pudo desasociar la memoria compartida\n");
    }

    // Eliminar el segmento de memoria compartida
    if (eliminarMemoria(shmid) == -1)
    {
        fprintf(stderr, "No se pudo eliminar la memoria compartida\n");
    }

    // Eliminar semáforo
    eliminarSemaforo(semid);

    return 0;
}
