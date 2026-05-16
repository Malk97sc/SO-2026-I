# Capítulo 4: Sincronización de Hilos (POSIX)

Este capítulo profundiza en los mecanismos esenciales para coordinar la ejecución de hilos y prevenir errores comunes en entornos concurrentes, como las condiciones de carrera.

## Objetivos del Capítulo
- Entender y resolver **Condiciones de Carrera** (Race Conditions).
- Implementar **Exclusión Mutua** mediante Mutexes.
- Sincronizar hilos en puntos de encuentro usando **Barreras**.
- Establecer comunicación entre hilos basada en estados con **Variables de Condición**.

## Contenido Detallado

### 1. [Exclusión Mutua (Mutex)](./1_mutex/)
Uso de `pthread_mutex_t` para proteger secciones críticas y garantizar que solo un hilo acceda a un recurso compartido a la vez.

### 2. [Barreras de Sincronización](./2_barreras/)
Implementación de puntos de control donde un grupo de hilos debe esperar a que todos lleguen antes de continuar.

### 3. [Variables de Condición](./3_variables_condicion/)
Mecanismos avanzados para que los hilos esperen señales sobre cambios en el estado de los datos compartidos.

## Cómo Compilar las Guías
Cada subdirectorio contiene un `Makefile` para generar los documentos PDF a partir de los archivos `.tex`. 

Para compilar todo el capítulo, ejecute desde la raíz de este directorio:
```bash
make
```

Para limpiar los archivos generados:
```bash
make clean
```
