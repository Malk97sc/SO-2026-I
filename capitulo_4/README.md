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

## Configuración Requerida

Para usar `pthread_barrier_t`, es necesario agregar la siguiente `defines` en el archivo `c_cpp_properties.json`:

```json
"defines": [
    "_XOPEN_SOURCE=700"
]
```

### Cómo agregar la macro _XOPEN_SOURCE=700

1. Presiona `Ctrl + Shift + P` para abrir la paleta de comandos en VS
2. Escribe `C/C++: Edit Configurations` y selecciona esa opción
3. Se abrirá el archivo `c_cpp_properties.json`
4. En la sección `"defines"`, agrega `"_XOPEN_SOURCE=700"`
5. Guarda el archivo

### ¿Por qué es necesario?

`_XOPEN_SOURCE=700` es una macro de compilación que activa las características **POSIX.1-2008** del estándar POSIX. Sin esta macro:

- El compilador no reconoce `pthread_barrier_t`, `pthread_barrier_init()`, etc.
- Obtendrás errores como "unknown type name 'pthread_barrier_t'" o "implicit declaration of function 'pthread_barrier_init'"
- Es una medida de seguridad del compilador: solo expone las funciones POSIX cuando se especifica explícitamente

Al agregar esta define, le indicamos al compilador (y al editor de VS Code) que queremos usar características POSIX modernas para sincronización de hilos.

