#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <ctype.h>

#define MAX_CLIENTES 5
#define MAX_MSG 150
int clientes_activos = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *atender_cliente(void *arg);
void procesar_mensaje(char *msg, char *respuesta);

int main() {
    int servidor_fd, nuevo_socket;
    struct sockaddr_in direccion;
    socklen_t addrlen = sizeof(direccion);

    servidor_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (servidor_fd == -1) {
        perror("Error al crear socket");
        exit(1);
    }

    direccion.sin_family = AF_INET;
    direccion.sin_addr.s_addr = INADDR_ANY;
    direccion.sin_port = htons(5000);

    if (bind(servidor_fd, (struct sockaddr *)&direccion, sizeof(direccion)) < 0) {
        perror("Error en bind");
        close(servidor_fd);
        exit(1);
    }

    listen(servidor_fd, MAX_CLIENTES);
    printf("Servidor escuchando en puerto 5000...\n");

    while (1) {
        nuevo_socket = accept(servidor_fd, (struct sockaddr *)&direccion, &addrlen);
        if (nuevo_socket < 0) {
            perror("Error en accept");
            continue;
        }

        pthread_mutex_lock(&mutex);
        if (clientes_activos >= MAX_CLIENTES) {
            pthread_mutex_unlock(&mutex);
            char *msg = "Servidor lleno, espere...\n";
            send(nuevo_socket, msg, strlen(msg), 0);
            close(nuevo_socket);
            continue;
        }

        clientes_activos++;
        printf("Nuevo cliente conectado. Total: %d\n", clientes_activos);
        pthread_mutex_unlock(&mutex);

        pthread_t hilo;
        int *nuevo_sock = malloc(sizeof(int));
        *nuevo_sock = nuevo_socket;
        pthread_create(&hilo, NULL, atender_cliente, nuevo_sock);
        pthread_detach(hilo);  // No necesitamos hacer join
    }

    close(servidor_fd);
    return 0;
}



void procesar_mensaje(char *msg, char *respuesta) {
    char comando[10];
    char texto[MAX_MSG];

    // Extraer comando y texto entre <>
    if (sscanf(msg, "%s <%[^>]>", comando, texto) != 2) {
        strcpy(respuesta, "Formato inválido. Usa: COMANDO <texto>");
        return;
    }

    if (strcmp(comando, "MAYUS") == 0) {
        for (int i = 0; texto[i]; i++)
            texto[i] = toupper(texto[i]);
        strcpy(respuesta, texto);
    }
    else if (strcmp(comando, "MIN") == 0) {
        for (int i = 0; texto[i]; i++)
            texto[i] = tolower(texto[i]);
        strcpy(respuesta, texto);
    }
    else if (strcmp(comando, "NORM") == 0) {
        texto[0] = toupper(texto[0]);
        for (int i = 1; texto[i]; i++)
            texto[i] = tolower(texto[i]);
        strcpy(respuesta, texto);
    }
    else if (strcmp(comando, "LONG") == 0) {
        sprintf(respuesta, "Longitud: %lu", strlen(texto));
    }
    else {
        strcpy(respuesta, "Comando no reconocido");
    }
}


void *atender_cliente(void *arg) {
    int sock = *(int *)arg;
    free(arg);
    char buffer[MAX_MSG];
    char respuesta[MAX_MSG];

    while (1) {
        int bytes = recv(sock, buffer, MAX_MSG - 1, 0);
        if (bytes <= 0) break;

        buffer[bytes] = '\0';

        if (strcmp(buffer, "EXIT") == 0) break;

        procesar_mensaje(buffer, respuesta);
        send(sock, respuesta, strlen(respuesta), 0);
    }

    close(sock);

    pthread_mutex_lock(&mutex);
    clientes_activos--;
    pthread_mutex_unlock(&mutex);

    printf("Cliente desconectado. Clientes activos: %d\n", clientes_activos);
    pthread_exit(NULL);
}

