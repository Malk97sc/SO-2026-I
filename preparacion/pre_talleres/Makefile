# Makefile para Pre-Talleres

TARGETS = taller_1_senales taller_2_tuberias taller_3_memoria taller_4_hilos_galaxias taller_5_hilos_ensamblaje
PDFS = $(addsuffix .pdf, $(TARGETS))

all: $(PDFS)

%.pdf: %.tex
	pdflatex $<
	pdflatex $<
	rm -f *.aux *.log *.out

clean:
	rm -f *.pdf *.aux *.log *.out

.PHONY: all clean
