# Capítulo 3: Hilos (POSIX Threads)

Este capítulo introduce el concepto de unidad de ejecución dentro de un proceso, permitiendo ejecutar múltiples tareas simultáneamente en un entorno compartido.

## Contenido

### 1. Introducción a Hilos (`pthreads`)

Conceptos básicos de hilos, creación (`pthread_create`) y la diferencia fundamental entre procesos e hilos.

### 2. Sincronización Básica (`pthread_join`)

Finalización de hilos, recolectar sus resultados y el estado de hilos destacados (`detached`).

### 3. Paso de Argumentos

Cómo pasar múltiples parámetros complejos a un hilo utilizando estructuras y punteros.

### 4. Modelo de Memoria en Hilos

Explica qué se comparte y qué es privado para cada hilo (Pila, Heap, Archivos, Variables Globales).