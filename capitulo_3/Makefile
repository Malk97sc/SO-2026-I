# capítulo 3: Hilos (Threads)

SUBDIRS = 1_creacion_hilos \
          2_finalizacion_hilos \
          3_paso_argumentos \
          4_memoria_hilos

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
