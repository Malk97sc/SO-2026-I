FILES = guia_funciones.pdf guia_argumentos_main.pdf

all: $(FILES)

%.pdf: %.tex
	pdflatex $<
	pdflatex $<
	rm -f *.aux *.log *.out

clean:
	rm -f *.pdf *.aux *.log *.out

.PHONY: all clean
