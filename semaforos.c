#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>
#include "semaforos.h"

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

int crearSemaforo() {
    int semid = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666);
    if (semid == -1) {
        perror("Error creando semáforo");
        exit(1);
    }
    return semid;
}

void inicializarSemaforo(int semid, int valor) {
    union semun sem_union;
    sem_union.val = valor;
    if (semctl(semid, 0, SETVAL, sem_union) == -1) {
        perror("Error inicializando semáforo");
        exit(1);
    }
}

void waitSemaforo(int semid) {
    struct sembuf sem_b = {0, -1, 0};
    if (semop(semid, &sem_b, 1) == -1) {
        perror("Error en wait (P)");
        exit(1);
    }
}

void signalSemaforo(int semid) {
    struct sembuf sem_b = {0, 1, 0};
    if (semop(semid, &sem_b, 1) == -1) {
        perror("Error en signal (V)");
        exit(1);
    }
}

void eliminarSemaforo(int semid) {
    if (semctl(semid, 0, IPC_RMID) == -1) {
        perror("Error eliminando semáforo");
    }
}