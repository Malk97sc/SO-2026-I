SUBDIRS = 1_señales \
          2_tuberias \
          3_memoria_compartida

all:
	@for dir in $(SUBDIRS); do \
		if [ -f $$dir/Makefile ]; then \
			echo "Compilando en $$dir..."; \
			$(MAKE) -C $$dir; \
		fi \
	done

clean:
	@for dir in $(SUBDIRS); do \
		if [ -f $$dir/Makefile ]; then \
			echo "Limpiando en $$dir..."; \
			$(MAKE) -C $$dir clean; \
		fi \
	done

.PHONY: all clean
