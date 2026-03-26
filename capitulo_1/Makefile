SUBDIRS = 1_creación_procesos \
          2_identificacion \
          3_estados_procesos \
          4_espera_procesos \
          5_ejercicios_propuestos

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