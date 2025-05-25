#ifndef MEMORIA_H_INCLUDED
#define MEMORIA_H_INCLUDED

#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int crearMemoriaCompartida(size_t tam);
void* asociarMemoria(int shmid);
int liberarMemoria(void *dir);
int eliminarMemoria(int shmid);

#endif // MEMORIA_H_INCLUDED
