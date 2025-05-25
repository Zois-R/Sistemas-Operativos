#include "memoria.h"


// Crea un segmento de memoria compartida y devuelve el ID del mismo
int crearMemoriaCompartida(size_t tam)
{
    int shmid = shmget(IPC_PRIVATE, tam, IPC_CREAT | 0666);
    if (shmid == -1)
    {
        perror("Error al crear la memoria compartida");
        return -1;
    }
    return shmid;
}

// Asocia el segmento de memoria compartida al espacio de direcciones del proceso
void* asociarMemoria(int shmid)
{
    void *dir = shmat(shmid, NULL, 0);
    if (dir == (void *) -1)
    {
        perror("Error al asociar la memoria compartida");
        return NULL;
    }
    return dir;
}

// Desasocia la memoria compartida del proceso
int liberarMemoria(void *dir)
{
    if (shmdt(dir) == -1)
    {
        perror("Error al desasociar la memoria compartida");
        return -1;
    }
    return 0;
}

// Elimina el segmento de memoria compartida completamente
int eliminarMemoria(int shmid)
{
    if (shmctl(shmid, IPC_RMID, NULL) == -1)
    {
        perror("Error al eliminar la memoria compartida");
        return -1;
    }
    return 0;
}
