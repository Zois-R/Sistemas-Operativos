#ifndef SEMAFOROS_H
#define SEMAFOROS_H

int crearSemaforo();
void inicializarSemaforo(int semid, int valor);
void waitSemaforo(int semid);
void signalSemaforo(int semid);
void eliminarSemaforo(int semid);

#endif