# Sistemas-Operativos

Ejercicio 1: Coordinación de Tareas con Procesos y Memoria Compartida
📝 Resumen
Este ejercicio implementa un sistema en C donde un proceso padre carga un archivo de empleados en memoria compartida. Luego, múltiples procesos hijos realizan operaciones concurrentes sobre esos datos, sincronizados mediante semáforos. El sistema se finaliza de forma ordenada al recibir la señal del padre.

⚙ Tecnología Usada
Lenguaje: C

Sistema operativo: Linux 

Herramientas: ps, top, htop, ipcs para monitoreo

💡 Diseño Funcional
✅ Creación de Procesos
El proceso principal (padre) inicia el programa.

Crea 4 procesos hijos usando fork().

✅ Memoria Compartida
Se reserva un segmento de memoria compartida para almacenar un vector de registros (tEmpleado).

Este vector es accesible por todos los procesos hijos.

✅ Operaciones de los Procesos Hijos
Cada hijo ejecuta una operación específica:

Calcular promedio de sueldos.

Aumentar sueldos por categoría.

Cambiar categoría según antigüedad.

Eliminar empleados con sueldos menores a un umbral.

Las operaciones se realizan en ciclos, hasta que el padre ordene finalizar.

✅ Sincronización
Se usan semáforos para evitar condiciones de carrera.

Antes de acceder a la memoria compartida, un proceso bloquea el semáforo; después de modificar, lo libera.

✅ Finalización del Programa
El padre espera una orden del usuario (entrada por teclado).

Envía señales a los hijos para que terminen.

Espera su finalización (wait).

Muestra el resultado final.

Libera memoria compartida, semáforos y otros recursos.

🖥 Monitoreo en Linux
ps, top, htop → ver procesos.

ipcs → ver memoria compartida y semáforos.

🏗 Estructura de Datos
c
Copiar
Editar
typedef struct {
    int legajo;
    char nombre[50];
    char categoria;
    int antiguedad;
    float sueldo;
} tEmpleado;
Ejercicio 2: Sistema Cliente-Servidor con Sockets y Threads
📝 Resumen
Este ejercicio desarrolla un sistema cliente-servidor en C usando sockets y threads.
El servidor puede atender hasta 5 clientes simultáneamente, quienes envían comandos y cadenas a procesar. La comunicación es por texto plano, con un protocolo simple.

⚙ Tecnología Usada
Lenguaje: C

Comunicación: Sockets TCP

Concurrencia: Threads (pthread)

Sistema operativo: Linux

💡 Diseño Funcional
✅ Inicio del Servidor
El servidor crea un socket TCP y queda a la espera de conexiones.

Límite máximo: 5 clientes simultáneos.

✅ Conexión de los Clientes
Cada cliente es un programa independiente.

Si el servidor tiene espacio, crea un nuevo thread para atenderlo.

Si no, el cliente espera.

✅ Comunicación
Los clientes envían mensajes con el formato:
<COMANDO> <TEXTO>
Ejemplo: MAYUS <hola mundo>

El servidor procesa y responde con el resultado.

✅ Comandos Disponibles
Comando	Descripción
MAYUS	Transforma texto a mayúsculas
MIN	Transforma texto a minúsculas
NORM	Normaliza la cadena (ej. quitar espacios)
LONG	Devuelve cantidad de caracteres

✅ Protocolo de Comunicación
Texto plano (máximo 150 caracteres por mensaje).

El texto a procesar va entre < y >.

✅ Finalización
Cliente puede desconectarse enviando EXIT.

Cuando todos los clientes se desconectan, el servidor cierra y libera recursos.

✅ Manejo de Errores
Si un cliente/servidor se cae, los demás siguen funcionando.

Mensajes demasiado largos son truncados o ignorados para evitar desbordes.

🖥 Monitoreo en Linux
netstat, lsof → sockets abiertos.

ps, top, htop → threads activos.
