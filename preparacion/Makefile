# Makefile de Entrenamiento General

DIRS = pre_talleres pre_parciales

all:
	@for dir in $(DIRS); do \
		$(MAKE) -C $$dir; \
	done

clean:
	@for dir in $(DIRS); do \
		$(MAKE) -C $$dir clean; \
	done

.PHONY: all clean
