# Ejercicio 1

## Compilación y ejecución

Asegurese de estar posicionado sobre /Ejercicio1, use `cd Ejercicio1`.

#### Compilar el programa
Ejecute en la terminal el siguiente comando:

```
gcc main.c archivos.c memoria.c procesos.c semaforos.c -o ejercicio1
```

#### Levantar el programa
Ejecute en la terminal el siguiente comando: 
```
./ejercicio1
```

## Funcionalidad

Al ejecutar el programa se cargará el lote de prueba en memoria y pedirá tocar la tecla "__enter__" para continuar.

Luego de darle a continuar, los procesos hijos realizarán acciones sobre el vector de empleados cargado en memoria hasta ingresar por teclado la tecla "__s__".

Por último se liberan los recursos y se imprime por pantalla como quedó el vector empleados con las operaciones realizadas.

# Ejercicio 2

Abra varias terminales, 1 para servidor y las que considere para cliente.

## Compilación y ejecución

Asegurese de estar posicionado sobre /Ejercicio2, use `cd Ejercicio2`.

### Servidor

#### Compilar el servidor

```
gcc servidor.c -o servidor
```

#### Levantar el servidor
Ejecute el siguiente comando en la terminal desiganada para servidor:
```
./servidor
```

### Clientes

#### Compilar el cliente

```
gcc cliente.c -o cliente
```

#### Levantar el cliente
Ejecute en una nueva terminal el siguiente comando: 
```
./cliente
```

## Funcionalidad

### Servidor

El servidor se encarga de brindarle una respuesta a los comandos que envíen los clientes.

Se aceptan hasta un máximo de 5 clientes. Superado dicho límite el cliente adicional no podrá comunicarse y finalizará su ejecución hasta que se libere un lugar en el servidor.

Si se terminan todas las conexiones, el servidor cerrará automáticamente.

### Cliente

> _El cliente se conecta al servidor por lo que debe levantar primero el servidor para poder utilizarlo_.

El cliente envía mensajes con el formato `COMANDO <texto>` que tienen una longitud máxima de 150 caracteres, el servidor responderá con el resultado de la operación o el error correspondiente. _Note que el texto va encerrado entre <>._

Para enviar estos mensajes se captura la entrada del teclado.

#### Comandos disponibles del cliente

- `MAYUS <texto>` devuelve _"texto"_ en mayúsculas.
- `MIN <texto>` devuelve _"texto"_ en minúsculas.
- `NORM <texto>` devuelve _"texto"_ normalizado.
- `LONG <texto>` devuelve la cantidad de caracteres que posee _"texto"_.
- `EXIT` finaliza la conexión con el servidor.