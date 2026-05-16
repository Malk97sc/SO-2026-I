# capítulo 4: Sincronización de Hilos

SUBDIRS = 1_mutex \
          2_barreras \
          3_variables_condicion

all:
	@for dir in $(SUBDIRS); do \
		if [ -d $$dir ]; then \
			if [ -f $$dir/Makefile ]; then \
				echo "Compilando en $$dir..."; \
				$(MAKE) -C $$dir; \
			fi \
		fi \
	done

clean:
	@for dir in $(SUBDIRS); do \
		if [ -d $$dir ]; then \
			if [ -f $$dir/Makefile ]; then \
				echo "Limpiando en $$dir..."; \
				$(MAKE) -C $$dir clean; \
			fi \
		fi \
	done

.PHONY: all clean
