#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <ctype.h>
#include <fcntl.h>

#define esLetra(x) ( ('A'<=(x) &&  (x)<='Z') || ('a'<=(x) &&  (x)<='z') )

int clientes_activos = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int leer_config(const char *ruta, int *puerto, int *max_clientes, int *max_msg) {
    FILE *f = fopen(ruta, "r");
    if (!f) {
        perror("No se pudo abrir el archivo de configuración");
        return 0;
    }

    char linea[100];
    int ok_puerto = 0, ok_clientes = 0, ok_msg = 0;

    while (fgets(linea, sizeof(linea), f)) {
        if (strncmp(linea, "PUERTO=", 7) == 0 && puerto) {
            *puerto = atoi(linea + 7);
            if (*puerto > 0) ok_puerto = 1;
        } 
        else if (strncmp(linea, "MAX_CLIENTES=", 13) == 0 && max_clientes) {
            *max_clientes = atoi(linea + 13);
            if (*max_clientes > 0) ok_clientes = 1;
        } 
        else if (strncmp(linea, "MAX_MSG=", 8) == 0 && max_msg) {
            *max_msg = atoi(linea + 8);
            if (*max_msg >= 150) ok_msg = 1;
        }
    }

    fclose(f);

    if (!ok_puerto || !ok_clientes || !ok_msg) {
        fprintf(stderr, "Error: Faltan parámetros obligatorios o son inválidos en el archivo de configuración.\n");
        return 0;
    }

    return 1;
}

void *atender_cliente(void *arg);
void procesar_mensaje(char *msg, char *respuesta, int max_msg);
char* normalizar_cadena(char* cad);
void obtener_comando_y_texto(char *msg, char *comando, char *texto, char *respuesta);

typedef struct {
    int sock;
    int max_msg;
} DatosCliente;

int main() {
    int servidor_fd, nuevo_socket;
    struct sockaddr_in direccion;
    socklen_t addrlen = sizeof(direccion);
    int puerto = 0, max_clientes = 0, max_msg = 0;
    int hubo_clientes = 0;

    if (!leer_config("config.txt", &puerto, &max_clientes, &max_msg)) {
        fprintf(stderr, "Servidor no se iniciará por configuración incompleta.\n");
        return 1;
    }

    servidor_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (servidor_fd == -1) {
        perror("Error al crear socket");
        exit(1);
    }

    direccion.sin_family = AF_INET;
    direccion.sin_addr.s_addr = INADDR_ANY;
    direccion.sin_port = htons(puerto);

    if (bind(servidor_fd, (struct sockaddr *)&direccion, sizeof(direccion)) < 0) {
        perror("Error en bind");
        close(servidor_fd);
        exit(1);
    }

    listen(servidor_fd, max_clientes);
    fcntl(servidor_fd, F_SETFL, O_NONBLOCK);

    printf("Servidor escuchando en puerto %d...\n", puerto);

    while (1) {
        nuevo_socket = accept(servidor_fd, (struct sockaddr *)&direccion, &addrlen);

        if (nuevo_socket >= 0) {
            pthread_mutex_lock(&mutex);
            if (clientes_activos >= max_clientes) {
                pthread_mutex_unlock(&mutex);
                char *msg = "Servidor lleno. Intente más tarde.";
                send(nuevo_socket, msg, strlen(msg), 0);
                close(nuevo_socket);
                continue;
            }

            clientes_activos++;
            hubo_clientes = 1;
            printf("Nuevo cliente conectado. Total: %d\n", clientes_activos);
            pthread_mutex_unlock(&mutex);

            pthread_t hilo;
            DatosCliente *datos = malloc(sizeof(DatosCliente));
            datos->sock = nuevo_socket;
            datos->max_msg = max_msg;
            pthread_create(&hilo, NULL, atender_cliente, datos);
            pthread_detach(hilo);
        } else {
            pthread_mutex_lock(&mutex);
            if (hubo_clientes && clientes_activos == 0) {
                pthread_mutex_unlock(&mutex);
                printf(">>> Cierre automático: no hay más clientes.\n");
                break;
            }
            pthread_mutex_unlock(&mutex);
            sleep(1);
        }
    }

    close(servidor_fd);
    return 0;
}

void procesar_mensaje(char *msg, char *respuesta, int max_msg) {
    char comando[10];
    char texto[max_msg];

    obtener_comando_y_texto(msg, comando, texto, respuesta);

    if (strcmp(comando, "MAYUS") == 0) {
        for (int i = 0; texto[i]; i++)
            texto[i] = toupper(texto[i]);
        strcpy(respuesta, texto);
    } else if (strcmp(comando, "MIN") == 0) {
        for (int i = 0; texto[i]; i++)
            texto[i] = tolower(texto[i]);
        strcpy(respuesta, texto);
    } else if (strcmp(comando, "NORM") == 0) {
        normalizar_cadena(texto);
        strcpy(respuesta, texto);
    } else if (strcmp(comando, "LONG") == 0) {
        sprintf(respuesta, "Longitud: %lu", strlen(texto));
    } else {
        strcpy(respuesta, "Comando no reconocido");
    }
}

void obtener_comando_y_texto(char *msg, char *comando, char *texto, char *respuesta) {
    msg[strcspn(msg, "\n")] = 0;
    char *inicioTexto = strchr(msg, ' ');
    if (inicioTexto != NULL && *(inicioTexto + 1) == '<') {
        size_t longitudComando = inicioTexto - msg;
        strncpy(comando, msg, longitudComando);
        comando[longitudComando] = '\0';

        char *finTexto = strrchr(inicioTexto + 2, '>');

        if (finTexto != NULL) {
            size_t longitudTexto = finTexto - (inicioTexto + 2);
            strncpy(texto, inicioTexto + 2, longitudTexto);
            texto[longitudTexto] = '\0';
        } else {
            strcpy(respuesta, "Formato inválido. Usa: COMANDO <texto>");
        }
    } else {
        strcpy(respuesta, "Formato inválido. Usa: COMANDO <texto>");
    }
}

void *atender_cliente(void *arg) {
    DatosCliente *datos = (DatosCliente *)arg;
    int sock = datos->sock;
    int max_msg = datos->max_msg;
    free(datos);

    char buffer[max_msg];
    char respuesta[max_msg];

    while (1) {
        int bytes = recv(sock, buffer, max_msg - 1, 0);
        if (bytes <= 0)
            break;

        buffer[bytes] = '\0';

        if (strcmp(buffer, "EXIT") == 0)
            break;

        procesar_mensaje(buffer, respuesta, max_msg);
        send(sock, respuesta, strlen(respuesta), 0);
    }

    close(sock);

    pthread_mutex_lock(&mutex);
    clientes_activos--;
    pthread_mutex_unlock(&mutex);

    printf("Cliente desconectado. Clientes activos: %d\n", clientes_activos);
    pthread_exit(NULL);
}

char* normalizar_cadena(char* cad) {
    int primerLetra = 0, cantLetras;
    char* posIni = cad;
    char* prevChar = NULL;

    while(*cad) {
        cantLetras = strlen(cad);
        if(esLetra(*cad)) {
            *cad = tolower(*cad);
            if(!primerLetra) {
                *cad = toupper(*cad);
                primerLetra = 1;
            }
            prevChar = cad;
        } else {
            if(*cad != ' ') {
                memmove(cad, cad+1, cantLetras);
                cad--;
            } else {
                if(!primerLetra || (prevChar != NULL && !esLetra(*prevChar))) {
                    memmove(cad, cad+1, cantLetras);
                    cad--;
                } else {
                    prevChar = cad;
                }
            }
        }
        cad++;
    }

    char* fin = posIni + strlen(posIni) - 1;
    if (fin >= posIni && *fin == ' ') {
        *fin = '\0';
    }

    return posIni;
}
