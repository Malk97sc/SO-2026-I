FILES = guia_archivos.pdf guia_lectura_archivos.pdf

all: $(FILES)

%.pdf: %.tex
	pdflatex $<
	pdflatex $<
	rm -f *.aux *.log *.out

clean:
	rm -f *.pdf *.aux *.log *.out

.PHONY: all clean
