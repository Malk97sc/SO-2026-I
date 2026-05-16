# Plantillas de Código

Este directorio contiene fragmentos de código reutilizables, diseñados para agilizar el desarrollo de ejercicios prácticos en la asignatura de Sistemas Operativos.

Las plantillas están organizadas por temas y proporcionan la lógica estructural básica (manejo de procesos, memoria dinámica, comunicación entre procesos, etc.) para que el estudiante pueda centrarse en la lógica específica de cada problema.

## Contenido de las Plantillas

### 1. Lectura de Archivos y Memoria (`lectura_archivos/`)
Implementaciones para la lectura de datos estructurados y el manejo eficiente de memoria dinámica para matrices y vectores.

### 2. Escritura de Archivos (`escritura_archivos/`)
Plantillas básicas para la persistencia de datos y creación de archivos de salida de texto.

### 3. Gestión de Procesos (`fork/`)
Ejemplos y estructuras comunes para la creación de jerarquías de procesos en sistemas POSIX.

### 4. Tuberías (`tuberias/`)
Mecanismos de comunicación entre procesos mediante pipes, incluyendo ejemplos de comunicación serializada y en anillo.

### 5. Memoria Compartida (`memoria_compartida/`)
Uso de segmentos de memoria compartida (SHM) para el intercambio de datos y estructuras complejas entre procesos.

### 6. Manejo de Señales (`señales/`)
Configuración de manejadores de señales para la sincronización y control de flujo entre procesos.

### 7. Hilos (`hilos/`)
Uso de la librería `pthread` para la creación y gestión de hilos (threads) y paralelismo dentro de un mismo proceso.

### 8. Sincronización (`sincronizacion/`)
Implementación de mecanismos de control como semáforos, mutexes y variables de condición para la coordinación de procesos e hilos.