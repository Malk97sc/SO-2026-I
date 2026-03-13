FILES=guia_memoria_dinamica guia_vector_dinamico guia_matriz_dinamica

all: $(FILES)

%: %.tex
	pdflatex $@.tex
	pdflatex $@.tex
	rm -f *.aux *.log *.out

clean:
	rm -f *.pdf *.aux *.log *.out

.PHONY: all clean
