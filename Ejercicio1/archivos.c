#include "archivos.h"

int crearArchivo(FILE **pf, const char *nombreArchivo)
{

    tEmpleado empleados[] =
    { ///Legajo   nombre     cat  ant    sueldo
        {1001, "Juan Perez", 'A', 5, 50000},
        {1002, "Maria Gomez", 'B', 3, 42000},
        {1003, "Carlos Ruiz", 'C', 10, 60000},
        {1004, "Ana Torres", 'B', 7, 47000},
        {1005, "Martin Fernandez", 'A', 4, 48000},
        {1006, "Lucia Rivas", 'C', 6, 51000},
        {1007, "Javier Diaz", 'A', 2, 39000},
        {1008, "Carolina Romero", 'B', 9, 53000},
        {1009, "Diego Sosa", 'C', 12, 62000},
        {1010, "Florencia Acura", 'B', 3, 44000},
        {1011, "Federico Lopez", 'A', 8, 56000},
        {1012, "Valentina Herrera", 'C', 11, 61000},
        {1013, "Santiago Medina", 'B', 5, 49000},
        {1014, "Agustina Vargas", 'C', 7, 52000},
        {1015, "Gonzalo Castro", 'A', 6, 47000},
        {1016, "Camila Molina", 'B', 4, 45000},
        {1017, "Nicolas Paredes", 'C', 9, 58000},
        {1018, "Julieta Navarro", 'A', 3, 43000},
        {1019, "Tomas Gimenez", 'B', 10, 60000},
        {1020, "Milagros Benitez", 'C', 8, 55000}
    };

    if(fwrite(empleados,sizeof(empleados),1,*pf))
      return TODO_OK;


  return ERR_ARCH;
}

void mostrarArchivo(FILE **pf,const char *nombreArchivo)
{
  tEmpleado emp;
  
  printf("LEGAJO     NOMBRE        CATEGORIA   ANTIGUEDAD    SUELDO\n");
  fread(&emp,sizeof(tEmpleado),1,*pf);
  while(!feof(*pf))
    {
        printf("%-7d %-20s %-3c %10d %14.2f\n",emp.legajo,emp.nombre,emp.categoria,emp.antiguedad,emp.sueldo);
        fread(&emp,sizeof(tEmpleado),1,*pf);
    }
}


int aperturaArchivo(FILE **pf, const char *nombreArchivo, const char *apertura)
{
   *pf = fopen(nombreArchivo,apertura);

    if(!*pf)
      {
        printf("Error al abir el archivo");
        return ERR_ARCH;
      }
  return TODO_OK;
}

void cargarMemoriaDesdeArchivo(FILE *pf, tEmpleado *empleados, int *cant)
{
  int i = 0;

  while(fread(&empleados[i],sizeof(tEmpleado),1,pf) == 1 && i < CANT_EMPLEADOS)
  {
    i++;
  }
  *cant = i; // son la cantidad real de empleados que leímos.
}