# capítulo 0: Introducción a C

SUBDIRS = 1_control_de_flujo \
          2_funciones \
          3_punteros \
          4_memoria_dinamica \
          5_archivos \
          prueba_diagnostica \
          taller_1_memoria

all:
	@for dir in $(SUBDIRS); do \
		echo "Compilando en $$dir..."; \
		$(MAKE) -C $$dir; \
	done

clean:
	@for dir in $(SUBDIRS); do \
		echo "Limpiando en $$dir..."; \
		$(MAKE) -C $$dir clean; \
	done

.PHONY: all clean
