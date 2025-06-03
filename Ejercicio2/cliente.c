#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAX_MSG 150

int main() {
    int sockfd;
    struct sockaddr_in serv_addr;
    char mensaje[MAX_MSG];
    char respuesta[MAX_MSG];

    // Crear socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Error al crear el socket");
        exit(1);
    }

    // Configurar dirección del servidor
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(5000);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Conectar al servidor
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Error al conectar al servidor");
        close(sockfd);
        exit(1);
    }

    printf("Conectado al servidor. Escribí comandos (EXIT para salir).\n");

    while (1) 
    {
        printf("\n> ");
        if (fgets(mensaje, MAX_MSG, stdin) == NULL) 
        {
            printf("Error al leer entrada.\n");
            continue;
        }

// Si no hay '\n' al final, significa que el input fue más largo que el buffer
        if (strchr(mensaje, '\n') == NULL) 
        {
            printf("El mensaje excede el límite de 150 caracteres y no será enviado. Por favor ingrese un mensaje menor al limite\n");

    // Limpiar el buffer de entrada
            int ch;
            while((ch = getchar()) != '\n' && ch != EOF);

            continue; // Volver a pedir mensaje
        }
//-------------------------------------------------------
        // Eliminar salto de línea
        mensaje[strcspn(mensaje, "\n")] = 0;

        if(strcmp(mensaje,"EXIT") != 0)
        {
            char *inicio = strchr(mensaje, '<');
            char *fin = strchr(mensaje, '>');
            // Verificar formato: debe tener < y > en orden correcto
            if (!inicio || !fin || fin < inicio) 
            {
                printf("El mensaje debe tener el formato: COMANDO <texto>\n");
                continue;
            }
        }

        // Enviar mensaje
        send(sockfd, mensaje, strlen(mensaje), 0);

        // Salir si el comando es EXIT
        if (strcmp(mensaje, "EXIT") == 0) 
        {
            printf("Cerrando conexión...\n");
            break;
        }

        // Recibir respuesta
        int bytes = recv(sockfd, respuesta, MAX_MSG - 1, 0);

        if (bytes <= 0) 
        {
            printf("El servidor cerró la conexión.\n");
            break;
        }
        if(strcmp(respuesta,"Servidor lleno. Intente más tarde.")==0)
        {
            printf("Respuesta del servidor: %s\n", respuesta);
            break;
        }

        respuesta[bytes] = '\0';
        printf("Respuesta del servidor: %s\n", respuesta);
    }

    close(sockfd);
    return 0;
}
