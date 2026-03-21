# Capítulo 1: Gestión de Procesos

Este capítulo profundiza en la creación, jerarquía y monitoreo de procesos en sistemas operativos basados en POSIX (Linux). Comprender cómo se gestionan los procesos es fundamental para entender el funcionamiento de un sistema operativo.

## Contenido

### 1. Creación de Procesos (`fork`)

Explica el funcionamiento de la llamada al sistema `fork()`, la bifurcación del flujo de ejecución y cómo distinguir entre el proceso padre y el proceso hijo.

### 2. Identificación de Procesos

Uso de los identificadores únicos de procesos (`PID`) y de procesos padre (`PPID`) mediante las funciones `getpid()` y `getppid()`.

### 3. Estados de Procesos

Análisis del ciclo de vida de un proceso, con especial énfasis en los estados de **Procesos Huérfanos** y **Procesos Zombies**.

### 4. Espera de Procesos (`wait`)

Uso de `wait()` y `waitpid()` para sincronizar la finalización de los hijos, permitiendo que el padre recoja sus códigos de salida y limpie la tabla de procesos.

### 5. Ejercicios Propuestos

Una serie de retos lógicos para construir jerarquías de procesos complejas y analizar el comportamiento de códigos que utilizan múltiples llamadas a `fork()`.
