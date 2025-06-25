#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/time.h>

int leer_config(const char *ruta, char *ip, int *puerto, int *max_clientes, int *max_msg) {
    FILE *f = fopen(ruta, "r");
    if (!f) {
        perror("No se pudo abrir el archivo de configuración");
        return 0;
    }

    char linea[100];
    int ok_puerto = 0, ok_clientes = 0, ok_msg = 0;
    while (fgets(linea, sizeof(linea), f)) {
        if (strncmp(linea, "IP=", 3) == 0) {
            strcpy(ip, linea + 3);
            ip[strcspn(ip, "\n")] = 0;
        } else if (strncmp(linea, "PUERTO=", 7) == 0) {
            *puerto = atoi(linea + 7);
            if (*puerto > 0) ok_puerto = 1;
        } else if (strncmp(linea, "MAX_CLIENTES=", 13) == 0) {
            *max_clientes = atoi(linea + 13);
            if (*max_clientes > 0) ok_clientes = 1;
        } else if (strncmp(linea, "MAX_MSG=", 8) == 0) {
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

int main() {
    int sockfd;
    struct sockaddr_in serv_addr;
    char ip[100];
    int puerto, max_clientes, max_msg;

    if (!leer_config("config.txt", ip, &puerto, &max_clientes, &max_msg)) {
        fprintf(stderr, "Cliente no se iniciará por configuración incompleta.\n");
        return 1;
    }

    char mensaje[max_msg];
    char respuesta[max_msg];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Error al crear el socket");
        exit(1);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(puerto);
    serv_addr.sin_addr.s_addr = inet_addr(ip);

    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Error al conectar al servidor");
        close(sockfd);
        exit(1);
    }

    struct timeval timeout;
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));

    int bytes = recv(sockfd, respuesta, max_msg - 1, 0);
    if (bytes > 0) {
        respuesta[bytes] = '\0';
        if (strcmp(respuesta, "Servidor lleno. Intente más tarde.") == 0) {
            printf("Respuesta del servidor: %s\n", respuesta);
            close(sockfd);
            return 0;
        }
    }

    timeout.tv_sec = 0;
    timeout.tv_usec = 0;
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));

    printf("Conectado al servidor. Escribí comandos (EXIT para salir).\n");

    while (1) {
        printf("\n> ");
        if (fgets(mensaje, max_msg, stdin) == NULL) {
            printf("Error al leer entrada.\n");
            continue;
        }

        if (strchr(mensaje, '\n') == NULL) {
            printf("El mensaje excede el límite y no será enviado.\n");
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF);
            continue;
        }

        mensaje[strcspn(mensaje, "\n")] = 0;

        if (strcmp(mensaje, "EXIT") != 0) {
            char *inicio = strchr(mensaje, '<');
            char *fin = strchr(mensaje, '>');
            if (!inicio || !fin || fin < inicio) {
                printf("El mensaje debe tener el formato: COMANDO <texto>\n");
                continue;
            }
        }

        send(sockfd, mensaje, strlen(mensaje), 0);

        if (strcmp(mensaje, "EXIT") == 0) {
            printf("Cerrando conexión...\n");
            break;
        }

        bytes = recv(sockfd, respuesta, max_msg - 1, 0);
        if (bytes <= 0) {
            printf("El servidor cerró la conexión.\n");
            break;
        }

        respuesta[bytes] = '\0';
        printf("Respuesta del servidor: %s\n", respuesta);
    }

    close(sockfd);
    return 0;
}
