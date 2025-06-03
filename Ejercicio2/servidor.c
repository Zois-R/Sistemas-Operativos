#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <ctype.h>
#include <fcntl.h>

#define MAX_CLIENTES 5
#define MAX_MSG 150

#define esLetra(x) ( ('A'<=(x) &&  (x)<='Z') || ('a'<=(x) &&  (x)<='z') )

int clientes_activos = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *atender_cliente(void *arg);
void procesar_mensaje(char *msg, char *respuesta);
char* normalizar_cadena(char* cad);
void obtener_comando_y_texto(char *msg, char *comando, char *texto, char *respuesta);


//-------------------------------------------------------
//MAIN PRINCIPAL
int main() {
    int servidor_fd, nuevo_socket;
    struct sockaddr_in direccion;
    socklen_t addrlen = sizeof(direccion);
    int hubo_clientes = 0;

    servidor_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (servidor_fd == -1) 
    {
        perror("Error al crear socket");
        exit(1);
    }

    direccion.sin_family = AF_INET;
    direccion.sin_addr.s_addr = INADDR_ANY;
    direccion.sin_port = htons(5000);

    if (bind(servidor_fd, (struct sockaddr *)&direccion, sizeof(direccion)) < 0) 
    {
        perror("Error en bind");
        close(servidor_fd);
        exit(1);
    }

    listen(servidor_fd, MAX_CLIENTES);

    fcntl(servidor_fd, F_SETFL, O_NONBLOCK);

    printf("Servidor escuchando en puerto 5000...\n");

    while (1) 
    {
        nuevo_socket = accept(servidor_fd, (struct sockaddr *)&direccion, &addrlen);

        if (nuevo_socket >= 0) 
        {
            pthread_mutex_lock(&mutex);
            if (clientes_activos >= MAX_CLIENTES) 
            {
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
            int *nuevo_sock = malloc(sizeof(int));
            *nuevo_sock = nuevo_socket;
            pthread_create(&hilo, NULL, atender_cliente, nuevo_sock);
            pthread_detach(hilo);
        } else 
        {
        // No hay nueva conexión, revisamos si ya no quedan clientes
            pthread_mutex_lock(&mutex);
            if (hubo_clientes && clientes_activos == 0) 
            {
                pthread_mutex_unlock(&mutex);
                printf(">>> Cierre automático: no hay más clientes.\n");
                return 0;
            }
            pthread_mutex_unlock(&mutex);
            sleep(1); // Esperar un segundo antes de intentar de nuevo
        }
    }


    close(servidor_fd);
    return 0;
}

//-------------------------------------------------------
void procesar_mensaje(char *msg, char *respuesta) {
    char comando[10];
    char texto[MAX_MSG];

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
//-----------------------------------------------------------------------
void obtener_comando_y_texto(char *msg, char *comando, char *texto, char *respuesta)
{
    // Elimina el salto de línea al final si existe
    msg[strcspn(msg, "\n")] = 0;

    // Busca el primer espacio para separar el comando
    char *inicioTexto = strchr(msg, ' ');
    if (inicioTexto != NULL && *(inicioTexto + 1) == '<') {
        // Copia el comando
        size_t longitudComando = inicioTexto - msg;
        strncpy(comando, msg, longitudComando);
        comando[longitudComando] = '\0';

        // Busca el final del texto entre <>
        char *finTexto = strrchr(inicioTexto + 2, '>');

        if (finTexto != NULL) {
            size_t longitudTexto = finTexto - (inicioTexto + 2);
            strncpy(texto, inicioTexto + 2, longitudTexto);
            texto[longitudTexto] = '\0';
        } else {
            // Error: Falta el signo '>' de cierre.
            strcpy(respuesta, "Formato inválido. Usa: COMANDO <texto>");
        }
    } else {
        strcpy(respuesta, "Formato inválido. Usa: COMANDO <texto>");
    }
}
//-----------------------------------------------------------------------
void *atender_cliente(void *arg) {
    int sock = *(int *)arg;
    free(arg);
    char buffer[MAX_MSG];
    char respuesta[MAX_MSG];

    while (1) {
        int bytes = recv(sock, buffer, MAX_MSG - 1, 0);
        if (bytes <= 0) 
            break;
        
        buffer[bytes] = '\0';

        if (strcmp(buffer, "EXIT") == 0)
            break;

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
//-----------------------------------------------------------------------
char* normalizar_cadena(char* cad)
{
    int primerLetra = 0, cantLetras;
    char* posIni = cad;
    char* prevChar = NULL;

    while(*cad)
    {
        cantLetras = strlen(cad);
        if(esLetra(*cad))
        {
            *cad = tolower(*cad);
            if(!primerLetra)
            {
                *cad = toupper(*cad);
                primerLetra = 1;
            }
            prevChar = cad;
        }
        else
        {
            if(*cad != ' ')
            {
                memmove(cad, cad+1, cantLetras);
                cad--;
            }
            else
            {
                if(!primerLetra || (prevChar != NULL && !esLetra(*prevChar)))
                {
                    memmove(cad, cad+1, cantLetras);
                    cad--;
                }
                else
                {
                    prevChar = cad;
                }
            }
        }
        cad++;
    }

    // Eliminar espacio final
    char* fin = posIni + strlen(posIni) - 1;
    if (fin >= posIni && *fin == ' ') {
        *fin = '\0';
    }

    return posIni;
}